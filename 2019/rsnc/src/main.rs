use nix::sys::select;
use nix::sys::select::FdSet;
use nix::sys::socket::*;
use nix::unistd;
use std::env;
use std::error::Error;
use std::os::unix::io::RawFd;
use std::vec::Vec;

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

struct SockBuf {
    infd: RawFd,
    outfd: RawFd,
    buf: Vec<u8>,
    cur: usize,
    end: usize,
}

impl SockBuf {
    fn new(infd: RawFd, outfd: RawFd, bufsz: usize) -> SockBuf {
        SockBuf {
            infd,
            outfd,
            buf: vec![0; bufsz],
            cur: 0,
            end: 0,
        }
    }
    fn empty(&self) -> bool {
        self.cur == self.end
    }
    fn read(&mut self) -> nix::Result<usize> {
        self.cur = 0;
        let result = unistd::read(self.infd, &mut self.buf);
        match result {
            Ok(size) => {
                self.end = size;
                result
            }
            Err(_) => result,
        }
    }
    fn write(&mut self) -> nix::Result<usize> {
        let result = unistd::write(self.outfd, &self.buf[0..self.end]);
        match result {
            Ok(size) => {
                self.cur += size;
                result
            }
            Err(_) => result,
        }
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
    let mut sbs = [
        SockBuf::new(0, connfd, buf_len),
        SockBuf::new(connfd, 1, buf_len),
    ];

    loop {
        let (mut rset, mut wset) = make_rwset(&sbs);
        let _nfds = select::select(maxfdp1, &mut rset, &mut wset, None, None);
        for p in sbs.iter_mut() {
            if rset.contains(p.infd) {
                if p.read()? == 0 {
                    return Ok(());
                }
            }
            if wset.contains(p.outfd) {
                if p.write()? == 0 {
                    return Ok(());
                }
            }
        }
    }
}
