use nix::sys::select::FdSet;
use nix::unistd;
use std::os::unix::io::RawFd;

pub struct SockBuf {
    infd: RawFd,
    outfd: RawFd,
    buf: Vec<u8>,
    cur: usize,
    end: usize,
}

impl SockBuf {
    pub fn new(infd: RawFd, outfd: RawFd, bufsz: usize) -> SockBuf {
        SockBuf {
            infd,
            outfd,
            buf: vec![0; bufsz],
            cur: 0,
            end: 0,
        }
    }
    pub fn infd(&self) -> RawFd {
        self.infd
    }
    pub fn outfd(&self) -> RawFd {
        self.outfd
    }
    pub fn empty(&self) -> bool {
        self.cur == self.end
    }
    pub fn read(&mut self) -> nix::Result<usize> {
        self.cur = 0;
        let result = unistd::read(self.infd, &mut self.buf);
        if let Ok(size) = result {
            self.end = size;
        }
        result
    }
    pub fn write(&mut self) -> nix::Result<usize> {
        let result = unistd::write(self.outfd, &self.buf[0..self.end]);
        if let Ok(size) = result {
            self.cur += size;
        }
        result
    }
}

pub fn make_rwset(sockbufs: &[SockBuf]) -> (FdSet, FdSet) {
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
