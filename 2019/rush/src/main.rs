use std::ffi::CString;

fn rush_repl() {
    use std::io::{stdin, stdout, Write};
    print!("$ ");
    let _ = stdout().flush();

    let mut line = String::new();
    stdin().read_line(&mut line).expect("could not read line");
    line.pop();
    let line = CString::new(line).expect("could not create CString");

    let res = nix::unistd::execvp(&line, &[&line.clone()]);
    let res = match res {
        Ok(_) => 1,
        Err(error) => panic!("{:?}", error),
    };
}

fn main() {
    rush_repl();
}
