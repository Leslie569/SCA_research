def group_lines(input_file, output_file, group_size=8):
    with open(input_file, 'r') as infile:
        lines = infile.readlines()  # Read all lines from the input file

    # Group the lines into chunks of the specified group size
    grouped_lines = [lines[i:i + group_size] for i in range(0, len(lines), group_size)]

    # Write the grouped lines to the output file
    with open(output_file, 'w') as outfile:
        for group in grouped_lines:
            # Join the group into a single string and write to the output file
            outfile.write(' '.join(line.strip() for line in group) + '\n')  # Use space as a separator

# Usage
input_file = 'ELMO/output/printdata.txt'  # Replace with your input file name
output_file = 'ELMO/output/output.txt'  # Replace with your desired output file name
group_lines(input_file, output_file)