#!/bin/bash

# Script to run the tinyc-compiler on example files and validate the output

# Check if the validator exists
if [ ! -f "./ast-validator" ]; then
    echo "Error: ast-validator not found in the current directory."
    exit 1
fi

# Check if the schema file exists
if [ ! -f "./tinyc-ast-schema.json" ]; then
    echo "Error: tinyc-ast-schema.json not found in the current directory."
    exit 1
fi

# Create output directory if it doesn't exist
mkdir -p examples

# Loop through example files from 0 to 15
for i in {0..15}; do
    example_file="../examples/${i}_*.tc"

    # Use find to get the exact filename (if it exists)
    file_path=$(find ../examples -name "${i}_*.tc" -type f | head -n 1)

    if [ -n "$file_path" ]; then
        # Extract just the filename from the path
        file_name=$(basename "$file_path")
        base_name="${file_name%.*}"
        output_file="./examples/${base_name}.json"

        echo "Processing $file_path -> $output_file"

        # Run the compiler and redirect output to the JSON file
        ../build/tinyc-compiler -p "$file_path" > "$output_file"

        # Validate the output
        ./ast-validator tinyc-ast-schema.json "$output_file"

        # Check the validator result
        if [ $? -eq 0 ]; then
            echo "✅ Validation passed for $base_name"
        else
            echo "❌ Validation failed for $base_name"
        fi

        echo ""
    else
        echo "Warning: No file found matching ${i}_*.tc"
    fi
done

echo "All tests completed!"