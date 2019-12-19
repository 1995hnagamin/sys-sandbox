use nix::sys::select;
use nix::sys::socket::*;
use nix::unistd;
use std::env;
use std::error::Error;
use std::os::unix::io::RawFd;
use std::vec::Vec;
mod sockbuf;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        println!("usage: rsnc <port>");
        return;
    }
    let portnum: u16 = args[1].parse().expect("not a number");
    println!("{}", portnum);
    run_server(portnum).unwrap();
}

fn run_server(portnum: u16) -> Result<(), Box<Error>> {
    let listenfd = socket(
        AddressFamily::Inet,
        SockType::Stream,
        SockFlag::empty(),
        SockProtocol::Tcp,
    )?;

    let ip_addr = IpAddr::new_v4(127, 0, 0, 1);
    let addr = SockAddr::new_inet(InetAddr::new(ip_addr, portnum));
    bind(listenfd, &addr)?;

    let backlog = 5;
    listen(listenfd, backlog)?;

    let connfd = accept(listenfd)?;
    handle_connection(connfd)?;
    unistd::close(connfd)?;
    unistd::close(listenfd)?;
    Ok(())
}

fn handle_connection(connfd: RawFd) -> Result<(), Box<Error>> {
    let maxfdp1 = 1 + connfd;
    let buf_len = 10;
    let mut sbs = [
        sockbuf::SockBuf::new(0, connfd, buf_len),
        sockbuf::SockBuf::new(connfd, 1, buf_len),
    ];

    loop {
        let (mut rset, mut wset) = sockbuf::make_rwset(&sbs);
        let _nfds = select::select(maxfdp1, &mut rset, &mut wset, None, None);
        for p in sbs.iter_mut() {
            if rset.contains(p.infd()) {
                if p.read()? == 0 {
                    return Ok(());
                }
            }
            if wset.contains(p.outfd()) {
                if p.write()? == 0 {
                    return Ok(());
                }
            }
        }
    }
}
