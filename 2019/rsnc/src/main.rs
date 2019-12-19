use nix::sys::select::*;
use nix::sys::socket::*;
use nix::unistd;
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
        let connfd = accept(listenfd)?;
        handle_connection(connfd)?;
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

fn make_rwset(sockbufs: &[SockBuf]) -> (FdSet, FdSet) {
    let mut rset = FdSet::new();
    let mut wset = FdSet::new();
    for sb in sockbufs {
        if sb.empty() {
            rset.insert(sb.infd)
        } else {
            wset.insert(sb.outfd)
        }
    }
    (rset, wset)
}

fn handle_connection(connfd: RawFd) -> Result<(), Box<Error>> {
    let maxfdp1 = 1 + connfd;
    let buf_len = 10;
    let mut sendbuf: std::vec::Vec<u8> = vec![0; buf_len];
    let mut recvbuf: std::vec::Vec<u8> = vec![0; buf_len];
    let mut sbs = [
        SockBuf::new(0, connfd, &mut sendbuf),
        SockBuf::new(connfd, 1, &mut recvbuf),
    ];

    loop {
        let (mut rset, mut wset) = make_rwset(&sbs);
        let _nfds = select(maxfdp1, &mut rset, &mut wset, None, None);
        for p in sbs.iter_mut() {
            if rset.contains(p.infd) {
                p.cur = 0;
                let size = unistd::read(p.infd, p.buf)?;
                if size == 0 {
                    return Ok(());
                }
                p.end = size;
            }
            if wset.contains(p.outfd) {
                let size = unistd::write(p.outfd, &p.buf[0..p.end])?;
                if size == 0 {
                    return Ok(());
                }
                p.cur += size;
            }
        }
    }
}
