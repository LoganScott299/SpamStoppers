The purpose of this repository is to house the source code and executable for SpamStoppers Project

***

## Installation / Usage

**Datasets**

Files recomemnded are the Subreddit Comments and Submissions

Datasets for each subreddit can be found at this link where user can torrent the specific file necessary

    https://academictorrents.com/details/30dee5f0406da7a353aff6a8caa2d54fd01f2ca1

To use the correct file format, all input files must be in the jsonl format.  However since the original datasets for each file are compressed using Zstandard compression (.zst extenstion), they need to be uncompressed and place into a data folder like:

Data/*Name of Subreddit*/*files*

After unzipping the files, you must add the file extension **.jsonl**
 

**Requirements**

g++ 15.2.0

7zip (or any other file extractor to unzip zst files)



**How to Run**

1. **Clone the Repo** — Add the project files to your system locally.
2. **Add dataset files** — Add the extracted files you want to analyze into the detailed datapath above ^^^.
3. **Run with Arguments** — Run SpamStoppers.exe with arguments detailing the path of the submissions and path of comments
           
        Usage: "SpamStoppers.exe  <submissions_unzipped_jsonl_path> <comments_unzipped_jsonl_path>"

### Example:

```
SpamStoppers.exe data/AITH/AITH_submissions.jsonl data/AITH/AITH_comments.jsonl

```
 

## Authors

- [Logan Scott](https://www.github.com/loganscott299)
- [Nicholas Reyes](https://www.github.com/spherohero)
- [Shravya Mandava](https://www.github.com/smandava0512)



