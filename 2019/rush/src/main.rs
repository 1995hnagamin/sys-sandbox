use nix::unistd;
use std::error::Error;
use std::ffi::CString;
mod parse;

enum Goal {
    Eos, // end of session
    Nop,
    Finished(i32),
}

fn main() {
    rush_repl();
}

fn rush_repl() {
    use Goal::*;
    loop {
        match rush_read_eval_print() {
            Ok(Nop) => (),
            Ok(Finished(stat)) => println!("status: {}", stat),
            Ok(Eos) => {
                println!("exit");
                std::process::exit(0)
            }
            Err(err) => println!("rush: {}", err.to_string()),
        }
    }
}

fn rush_read_eval_print() -> Result<Goal, Box<Error>> {
    use std::io::{stdin, stdout, Write};
    use Goal::*;
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
    if cmd.len() == 0 {
        return Ok(Nop);
    }

    use unistd::ForkResult;
    match unistd::fork()? {
        ForkResult::Child => {
            let cmd: Vec<_> = cmd.iter().map(|word| word.as_c_str()).collect();
            let err = unistd::execvp(cmd[0], &cmd).unwrap_err();
            println!("rush: {}", err.to_string());
            std::process::exit(1)
        }
        ForkResult::Parent { child: _ } => wait_for_children(),
    }
}

fn wait_for_children() -> Result<Goal, Box<Error>> {
    use nix::sys::wait;
    use wait::WaitStatus::*;
    match wait::wait()? {
        Exited(_, stat) => Ok(Goal::Finished(stat)),
        _ => panic!("unimplemented"),
    }
}
