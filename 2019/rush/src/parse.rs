pub fn parse(line: &str) -> Result<Vec<&str>, String> {
    Ok(line.split_whitespace().collect())
}
