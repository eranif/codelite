#!/usr/bin/env python3
"""
Script to check for non-ASCII characters in a source file.
Reports the line and column position of any non-ASCII characters found.
"""

import sys
import argparse


def check_for_non_ascii(filepath):
    """
    Check a file for non-ASCII characters.
    
    Args:
        filepath: Path to the file to check
        
    Returns:
        List of tuples containing (line_number, column_number, character, line_text)
    """
    non_ascii_chars = []
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            for line_num, line in enumerate(f, start=1):
                for col_num, char in enumerate(line, start=1):
                    if ord(char) > 127:  # ASCII characters are 0-127
                        non_ascii_chars.append((line_num, col_num, char, line.rstrip()))
        
        return non_ascii_chars
    
    except FileNotFoundError:
        print(f"Error: File '{filepath}' not found.", file=sys.stderr)
        sys.exit(1)
    except PermissionError:
        print(f"Error: Permission denied to read '{filepath}'.", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error reading file: {e}", file=sys.stderr)
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(
        description='Check a source file for non-ASCII characters.'
    )
    parser.add_argument(
        'filepath',
        help='Path to the source file to check'
    )
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Show the full line containing non-ASCII characters'
    )
    
    args = parser.parse_args()
    
    non_ascii_chars = check_for_non_ascii(args.filepath)
    
    if not non_ascii_chars:
        print(f"✓ No non-ASCII characters found in '{args.filepath}'")
        return 0
    
    print(f"✗ Found {len(non_ascii_chars)} non-ASCII character(s) in '{args.filepath}':\n")
    
    for line_num, col_num, char, line_text in non_ascii_chars:
        print(f"  Line {line_num}, Column {col_num}: '{char}' (U+{ord(char):04X})")
        if args.verbose:
            print(f"    {line_text}")
            print(f"    {' ' * (col_num - 1)}^")
    
    return 1


if __name__ == '__main__':
    sys.exit(main())
