use nix::sys::socket::*;
use std::env;
use std::error::Error;
use std::os::unix::io::RawFd;

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

    loop {
        let connfd = accept(listenfd);
        handle_connection(connfd);
    }
}

struct SockBuf<'a> {
    infd: RawFd,
    outfd: RawFd,
    buf: &'a mut [u8],
    cur: usize,
    end: usize,
}

impl<'a> SockBuf<'a> {
    fn new(infd: RawFd, outfd: RawFd, buf: &mut [u8]) -> SockBuf {
        SockBuf {
            infd,
            outfd,
            buf,
            cur: 0,
            end: 0,
        }
    }
    fn empty(&self) -> bool {
        self.cur == self.end
    }
}