use std::error::Error;
use std::ffi::CString;
mod parse;

fn rush_read_eval_print() -> Result<(), Box<Error>> {
    use nix::unistd::ForkResult;
    use std::io::{stdin, stdout, Write};
    print!("$ ");
    stdout().flush()?;

    let mut line = String::new();
    if stdin().read_line(&mut line)? == 0 {
        println!("exit");
        std::process::exit(0);
    }
    line.pop();
    let cmd = parse::parse(&line).and_then(|words| {
        words
            .iter()
            .copied()
            .map(|word| CString::new(word))
            .collect::<Result<Vec<_>, _>>()
            .map_err(|err| err.to_string())
    })?;
    if cmd.len() < 1 {
        return Ok(());
    }

    match nix::unistd::fork()? {
        ForkResult::Child => {
            let cmd: Vec<_> = cmd.iter().map(|word| word.as_c_str()).collect();
            nix::unistd::execvp(cmd[0], &cmd)?;
        }
        ForkResult::Parent { child: _ } => {
            nix::sys::wait::wait()?;
        }
    };
    Ok(())
}

fn rush_repl() {
    loop {
        match rush_read_eval_print() {
            Ok(()) => (),
            Err(err) => println!("rush: {}", err.to_string()),
        }
    }
}

fn main() {
    rush_repl();
}
