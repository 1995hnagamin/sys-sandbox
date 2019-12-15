use std::ffi::CString;
mod parse;

fn rush_repl() {
    use nix::unistd::ForkResult;
    use std::io::{stdin, stdout, Write};
    loop {
        print!("$ ");
        let _ = stdout().flush();

        let mut line = String::new();
        stdin().read_line(&mut line).expect("could not read line");
        line.pop();
        let cmd = parse::parse(&line)
            .and_then(|words| {
                words
                    .iter()
                    .copied()
                    .map(|word| CString::new(word))
                    .collect::<Result<Vec<_>, _>>()
                    .map_err(|err| err.to_string())
            })
            .unwrap();

        match nix::unistd::fork() {
            Ok(ForkResult::Child) => {
                let cmd: Vec<_> = cmd.iter().map(|word| word.as_c_str()).collect();
                let _ = nix::unistd::execvp(cmd[0], &cmd).expect("execvp error");
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
