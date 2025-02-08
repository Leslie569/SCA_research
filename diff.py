def count_differences(file1_path, file2_path):
    with open(file1_path, 'r') as file1, open(file2_path, 'r') as file2:
        lines1 = file1.readlines()
        lines2 = file2.readlines()

    # Ensure the two files have the same number of lines for comparison
    min_lines = min(len(lines1), len(lines2))
    differences = 0

    for i in range(min_lines):
        if lines1[i].strip() != lines2[i].strip():  # Strip whitespace for accurate comparison
            differences += 1

    # Count any remaining lines in the longer file as differences
    differences += abs(len(lines1) - len(lines2))

    return differences

# Example usage
file1 = 'ELMO/output/output.txt'  # Replace with your first file name
file2 = 'ELMO/output/ref_output.txt'  # Replace with your second file name
diff_count = count_differences(file1, file2)
print(f'Number of differing lines: {diff_count}')