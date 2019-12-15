use std::error::Error;
use std::ffi::CString;
mod parse;

enum Goal {
    Eos, // end of session
    Nop,
    Finished,
}

fn rush_read_eval_print() -> Result<Goal, Box<Error>> {
    use nix::unistd::ForkResult;
    use std::io::{stdin, stdout, Write};
    use Goal::{Eos, Finished, Nop};
    print!("$ ");
    stdout().flush()?;

    let mut line = String::new();
    if stdin().read_line(&mut line)? == 0 {
        return Ok(Eos);
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
        return Ok(Nop);
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
    Ok(Finished)
}

fn rush_repl() {
    use Goal::{Eos, Finished, Nop};
    loop {
        match rush_read_eval_print() {
            Ok(Nop) => (),
            Ok(Finished) => (),
            Ok(Eos) => {
                println!("exit");
                std::process::exit(0);
            }
            Err(err) => println!("rush: {}", err.to_string()),
        }
    }
}

fn main() {
    rush_repl();
}
