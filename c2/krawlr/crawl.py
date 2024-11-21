import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse
from collections import defaultdict
import json

def crawl_website(start_url, max_depth=2):
    visited = set()
    url_map = defaultdict(dict)

    def get_assets(url, soup):
        assets = {
            "images": [],
            "scripts": [],
            "stylesheets": [],
            "favicon": None,
        }

        for img in soup.find_all("img", src=True):
            assets["images"].append(urljoin(url, img["src"]))

        for script in soup.find_all("script", src=True):
            assets["scripts"].append(urljoin(url, script["src"]))

        for link in soup.find_all("link", rel=True, href=True):
            if "stylesheet" in link["rel"]:
                assets["stylesheets"].append(urljoin(url, link["href"]))

        favicon = soup.find("link", rel=["icon", "shortcut icon"])
        if favicon and favicon.get("href"):
            assets["favicon"] = urljoin(url, favicon["href"])
        elif not assets["favicon"]:  
            assets["favicon"] = urljoin(url, "/favicon.ico")

        return assets

    def crawl(url, depth):
        if url in visited or depth > max_depth:
            return
        visited.add(url)
        
        try:
            response = requests.get(url, timeout=10)
            response.raise_for_status()
            soup = BeautifulSoup(response.text, "html.parser")
            url_map[url]["assets"] = get_assets(url, soup)
            url_map[url]["links"] = []

            for a_tag in soup.find_all("a", href=True):
                link = urljoin(url, a_tag["href"])
                if urlparse(link).netloc == urlparse(start_url).netloc:  
                    url_map[url]["links"].append(link)
                    crawl(link, depth + 1)
        except requests.RequestException as e:
            print(f"Failed to crawl {url}: {e}")

    crawl(start_url, 0)
    return url_map

if __name__ == "__main__":
    start_url = input("Enter the starting URL (e.g., https://example.com): ").strip()
    max_depth = int(input("Enter the maximum depth to crawl: ").strip())
    
    site_map = crawl_website(start_url, max_depth)
    
    output_file = "site_map.json"
    with open(output_file, "w") as f:
        json.dump(site_map, f, indent=4)
    print(f"Site map saved to {output_file}")

