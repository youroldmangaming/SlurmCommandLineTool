```markdown
# Slurm Command Execution Tool

This tool allows users to execute commands across a Slurm-managed cluster while providing options to exclude specific nodes. It captures and displays the output from each node, making it easy to manage and monitor distributed tasks.

## Features

- Execute any command on all available nodes in the Slurm cluster.
- Exclude specific nodes from execution.
- Outputs the hostname along with the command's output for easy identification.
- Handles errors gracefully and provides informative feedback.

## Prerequisites

- A working Slurm cluster.
- GCC or another C compiler installed on your system.

## Installation

1. Clone or download this repository.
2. Navigate to the directory containing the source code.
3. Compile the program using GCC:

   ```bash
     gcc -o ... slurm_cluster.c
     sudo mv ... /usr/local/bin/

   ```

## Usage

The tool can be executed from the command line with the following syntax:

```bash
... [-e exclude_list] <command>
```

### Parameters

- `-e exclude_list`: (Optional) A comma-separated list of nodes to exclude from execution.
- `<command>`: The command you want to execute on the nodes.

### Examples

1. **Run a command on all available nodes:**

   ```bash
   ... date
   ```

   Output:
   ```
   rpi53: Sun Oct 27 18:17:00 NZDT 2024
   rpi54: Sun Oct 27 18:17:00 NZDT 2024
   mini: Sun Oct 27 06:17:00 PM NZDT 2024
   ```

2. **Run a command while excluding specific nodes:**

   ```bash
   ... -e "rpi41,rpi51" date
   ```

   Output:
   ```
   rpi53: Sun Oct 27 18:31:01 NZDT 2024
   rpi54: Sun Oct 27 18:31:01 NZDT 2024
   ```

### Error Handling

If there are issues with node allocation or command execution, the tool will provide informative error messages, including:

- Insufficient resources available.
- Excluded all available nodes.
- Permission issues.

## License

This tool is open-source and can be freely used and modified. Please refer to the license file for more details.

## Contributing

Contributions are welcome! If you have suggestions for improvements or find bugs, please open an issue or submit a pull request.

---

For any further questions or support, please contact the maintainer Mike Wilson, admin@youroldmangaming.com .
```

### Instructions for Use

1. Copy the above Markdown text into a file named `README.md`.
2. Place this file in the same directory as your compiled program (`slurm_run`).
3. You can view this README using any Markdown viewer or by rendering it on platforms like GitHub.

This README provides clear instructions on how to use your tool, including examples and error handling information, making it easy for users to understand its functionality.
