use std::ffi::CString;

fn rush_repl() {
    use nix::unistd::ForkResult;
    use std::io::{stdin, stdout, Write};
    loop {
        print!("$ ");
        let _ = stdout().flush();

        let mut line = String::new();
        stdin().read_line(&mut line).expect("could not read line");
        line.pop();
        let line = CString::new(line).expect("could not create CString");

        match nix::unistd::fork() {
            Ok(ForkResult::Child) => {
                let _ = nix::unistd::execvp(&line, &[&line.clone()]).expect("execvp error");
            }
            Ok(ForkResult::Parent { child: _ }) => {
                let _ = nix::sys::wait::wait();
            }
            Err(_) => panic!("fork error"),
        }
    }
}

fn main() {
    rush_repl();
}
