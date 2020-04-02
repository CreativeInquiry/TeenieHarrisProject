# Code

---
### cmoa-img-desc-parallel

Code to scrape image descriptions from the Carnegie Museum of Art website, using a spoofed browser with Selenium. 

- [`scraper.py`](cmoa-img-desc-parallel/scraper) Main script using selenium and Chrome driver to scrape image descriptions from the Carnegie Museum of Art website. Commandline argument: ```box name```. See below for batch scraping.
- [`generate_batch.py`](cmoa-img-desc-parallel/generate_batch) Generate shell scripts to batch scrape in parallel. The generated scripts are in `/batch` folder, and each of them can be run in a different terminal window.

---