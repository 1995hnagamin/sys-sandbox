extern crate pnet;

struct IcmpEchoMsg {
    icmp_type: u8,
    code: u8,
    checksum: u16,
    ident: u16,
    seqno: u16,
    data: Vec<u8>,
}

fn main() {
    println!("Hello, world!");
}
