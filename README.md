# JSONify

JSONify is a C++ command-line tool for parsing, formatting, and linting JSON and JSONC (JSON with comments) files. It provides a robust and extensible framework for processing JSON data with customizable formatting options and error checking.

## Features

- **Parse JSON/JSONC**: Parse JSON strings or files into an in-memory representation.
- **Format JSON**: Output JSON in pretty-printed or compact formats with customizable indentation.
- **Lint JSON**: Check for issues such as invalid numbers (infinity/NaN) or duplicate keys in objects.
- **JSONC Support**: Optionally process JSONC files by stripping single-line (`//`) comments.
- **Command-line Interface**: Easy-to-use options for linting, formatting, and configuring output.

## Requirements

- C++11 or later
- Standard C++ libraries (no external dependencies)
- Compiler supporting C++11 (e.g., GCC, Clang, MSVC)

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/jsonify.git
   cd jsonify
   ```

2. Compile the source code:
   ```bash
   g++ -std=c++11 main.cpp jsonparser.cpp jsonformatter.cpp jsonlinter.cpp -o jsonify
   ```

3. (Optional) Move the executable to a system path:
   ```bash
   sudo mv jsonify /usr/local/bin/
   ```

## Usage

Run `jsonify` with a JSON file and optional flags:

```bash
jsonify [options] <file.json>
```

### Options

- `--lint`: Lint the JSON file for issues (e.g., invalid numbers, duplicate keys).
- `--format`: Format the JSON file (pretty-printed by default).
- `--compact`: Output JSON in compact format (no indentation or newlines).
- `--indent N`: Set the number of spaces for indentation (default: 2).
- `--jsonc`: Allow JSONC files with `//` comments.
- `--help`: Display usage information.


## Linting Rules

When you use the `--lint` option, the linter checks your JSON for the following issues:

- **Empty Keys**: Flags objects that contain empty string keys (e.g., "": "value").
- **Deep Nesting**: Warns if JSON nesting exceeds 20 levels, helping to prevent stack overflow and mitigate potential Denial of Service (DoS) risks.
- **Naming Conventions**: Checks that object keys follow a consistent style, such as camelCase or snake_case (configurable or enforced by project settings).
- **Array Consistency**: Detects and warns when an array contains mixed data types (e.g., `[1, "two", 3]`), which may indicate a schema error.
- **Invalid Numbers**: Flags invalid numbers such as `Infinity` or `NaN`.
- **Duplicate Keys**: Detects duplicate keys in objects.

These rules help ensure your JSON is robust, consistent, and safe for use in production environments.

### Examples

- Format a JSON file with default indentation:
  ```bash
  ./jsonify --format input.json
  ```

- Format a JSON file in compact mode:
  ```bash
  ./jsonify --format --compact input.json
  ```

- Lint a JSON file for issues:
  ```bash
  ./jsonify --lint input.json
  ```

- Parse and format a JSONC file:
  ```bash
  ./jsonify --jsonc --format input.jsonc
  ```

- Format with custom indentation:
  ```bash
  ./jsonify --format --indent 4 input.json
  ```

## File Structure

- `jsonparser.h` / `jsonparser.cpp`: JSON parsing logic, including support for JSONC and Unicode escape sequences.
- `jsonformatter.h` / `jsonformatter.cpp`: JSON formatting with pretty-printed or compact output.
- `jsonlinter.h` / `jsonlinter.cpp`: JSON linting for detecting issues like invalid numbers or duplicate keys.
- `main.cpp`: Command-line interface for the `jsonify` tool.

## Example JSON Input

```json
{
  "name": "John Doe",
  "age": 30,
  "scores": [85, 90, 95],
  "address": {
    "street": "123 Main St",
    "city": "Anytown"
  }
}
```

### Pretty-Printed Output

```bash
./jsonify --format input.json
```

```json
{
  "name": "John Doe",
  "age": 30,
  "scores": [
    85,
    90,
    95
  ],
  "address": {
    "street": "123 Main St",
    "city": "Anytown"
  }
}
```

### Compact Output

```bash
./jsonify --format --compact input.json
```

```json
{"name":"John Doe","age":30,"scores":[85,90,95],"address":{"street":"123 Main St","city":"Anytown"}}
```

### Linting Output

```bash
./jsonify --lint invalid.json
```

Example output for a JSON with duplicate keys:
```
Warning: Duplicate key: name (line -1, column -1)
```

## Limitations

- JSONC support is limited to single-line `//` comments (multi-line `/* */` comments are not supported).
- No line/column tracking for linting errors (currently reported as `-1`).
- No support for streaming parsing of large JSON files.
- Basic auto-correction for missing commas (may not handle all malformed JSON cases).

## Future Improvements

- Add line/column tracking for precise error reporting.
- Support multi-line comments in JSONC mode.
- Implement streaming parsing for large JSON files.
- Extend linting with additional rules (e.g., schema validation, type checking).
- Allow output redirection to a file.
- Add unit tests for robustness.

## Contributing

Contributions are welcome! Please submit a pull request or open an issue on GitHub to report bugs or suggest features.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.