# Sample text files for testing typewriters and printers

## IMPORTANT WORDS ABOUT LINE ENDINGS

**lt;dr**: Any files in this folder that end with `.crlf.txt` use "CR LF" line endings and
are suitable for sending to your typewriter.

Panasonic KX-R typewriters expect the
[end of lines](https://en.wikipedia.org/wiki/Newline#Representation) to be
marked in the "CR LF" (aka "MS-DOS) style.

This means each line nees to end with a "Carriage Return" (CR) character (`\r`,
13, 0x0D) AND a "Newline" (LF) character (`\n`, 10, 0x0A), in that order.

If you use only CR, the carriage will return to the left margin but remain on
the current line. Likewise if you use only LR, the platten will advance one line
but the carriage will remain where is was.

If you're using a modern Macintosh or Linux, your line endings will be "LF" only
by default. If you are using a "Classic" Macintosh (OS 9 or older), your line
endings will be "CR" only. In either of these cases, any other files besides the
samples in this folder will need to be converted by a program like `unix2dos`.

### Nuclear Warning Messages

* [nuclear_warnings.crlf.txt](./nuclear_warnings.crlf.txt)

Text take from the Wikipedia page on
[Long-term nuclear waste warning
messages](https://en.wikipedia.org/wiki/Long-term_nuclear_waste_warning_messages).
This is the text used in the YouTube video.

### Line Length Tests

9 lines of test data (mostly spaces), with the specified number of columns each
(not including CR LR at end).

* [line_length_80.crlf.txt](./line_length_80.crlf.txt) - 80 columns, for 10 CPI setting.
* [line_length_96.crlf.txt](./line_length_96.crlf.txt) - 96 columns, for 12 CPI setting.
* [line_length_120.crlf.txt](./line_length_120.crlf.txt) - 120 columns, for 15 CPI setting.