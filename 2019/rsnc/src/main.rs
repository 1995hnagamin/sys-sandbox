extern crate clap;
use clap::{App, Arg};
use libc::{STDIN_FILENO, STDOUT_FILENO};
use nix::sys::select;
use nix::sys::socket::*;
use nix::unistd;
use std::error::Error;
use std::os::unix::io::RawFd;
mod sockbuf;

const BACKLOG_SIZE: usize = 5;
const SOCKBUF_SIZE: usize = 10;

fn main() {
    let app = App::new("rsnc").arg(
        Arg::with_name("port")
            .short("l")
            .long("listen")
            .takes_value(true),
    );
    let matches = app.get_matches();
    if let Some(port) = matches.value_of("port") {
        let portnum: u16 = port.parse().expect("not a number");
        println!("{}", portnum);
        run_server(portnum).unwrap();
        return;
    }
    println!("usage: rsnc -l <port>");
    return;
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

    listen(listenfd, BACKLOG_SIZE)?;

    let connfd = accept(listenfd)?;
    handle_connection(connfd)?;
    unistd::close(connfd)?;
    unistd::close(listenfd)?;
    Ok(())
}

fn handle_connection(connfd: RawFd) -> Result<(), Box<Error>> {
    let fds = [STDIN_FILENO, STDOUT_FILENO, connfd];
    let maxfdp1 = 1 + fds.iter().max().unwrap();
    let mut sbs = [
        sockbuf::SockBuf::new(STDIN_FILENO, connfd, SOCKBUF_SIZE),
        sockbuf::SockBuf::new(connfd, STDOUT_FILENO, SOCKBUF_SIZE),
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
