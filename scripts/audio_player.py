import ytmusicapi as yt
import readchar
import os
import webbrowser
from colorama import init, Fore, Back, Style
from PIL import Image
import requests
from io import BytesIO
import numpy as np
from sklearn.cluster import KMeans
import serial
import time
import random

PORT = '/dev/ttyACM0'
BAUD = 9600

atmega = serial.Serial(PORT,BAUD,timeout=.1)

def usart_send(color):
    random.shuffle(color)
    print(color)
    for sub_l in color:
        for i in sub_l:
            data = str(i) + '-'
            atmega.write(data.encode())
            print("sent : ",data)
    time.sleep(1)

def create_bw_mask(image_array):
    black_lower = (0, 0, 0)
    black_upper = (50, 50, 50)
    white_lower = (200, 200, 200)
    white_upper = (255, 255, 255)
    

    mask = np.zeros((image_array.shape[0], image_array.shape[1]), dtype=np.uint8)

    for x in range(image_array.shape[1]):
        for y in range(image_array.shape[0]):
            r, g, b = image_array[y, x]
            if (black_lower[0] <= r <= black_upper[0] and black_lower[1] <= g <= black_upper[1] and black_lower[2] <= b <= black_upper[2]) or \
               (white_lower[0] <= r <= white_upper[0] and white_lower[1] <= g <= white_upper[1] and white_lower[2] <= b <= white_upper[2]):
                mask[y, x] = 255  

    return mask

def get_dominant_colors(image_array, mask, n_colors=3):
 
    masked_pixels = image_array[mask == 0]  

    kmeans = KMeans(n_clusters=n_colors)
    kmeans.fit(masked_pixels)

    dominant_colors = kmeans.cluster_centers_.astype(int)
    return dominant_colors


class music:
    def __init__(self):
        init(autoreset=True)
        self.api = yt.YTMusic()
        self.index = 0
        self.songs = []
        self.results = []
        self.max_search = 30
        self.type = ""
        self.url = ""
        
    def search(self):
        print("for filtering songs prefix the name with '$' e.g $echoes")
        print("for filtering albums prefix the name with '@' e.g @Damnnation")
        print("#Search:")
        self.results.clear()
        self.songs.clear()
        query = input()
        while len(query) < 1:
            query = input()
        if(query[0]) == '$':
            query = query[1:]
            self.results = self.api.search(query,filter="songs",limit=self.max_search)
        elif(query[0]) == '@':
            query = query[1:]
            self.results = self.api.search(query,filter="albums",limit=self.max_search)
        else:
            self.results = self.api.search(query,limit=self.max_search)
        self.index = 0
        for i,song in enumerate(self.results):
            if (song["resultType"] != "artist") and (song["resultType"] != "video") and (song["resultType"] != "podcast") and (song["resultType"] != "profile") and (song["resultType"] != "playlist"):
                self.songs.append(song)
        self.print_search()
    
    def print_search(self):
        os.system('clear' if os.name == 'nt' else "printf '\033c'")
        print(self.index)
        for i,song in enumerate(self.songs):
            disp =  str(i) + "=> " + song["resultType"] + "-" + song["title"]
            if i == self.index:
                print(Back.RED + disp)
            else:
                print(disp)
    
    

    def color_extract(self):
        thumb_url = self.songs[self.index]['thumbnails'][0]['url']
        response = requests.get(thumb_url)
        thumb = Image.open(BytesIO(response.content)).convert("RGB")
        image_array = np.asarray(thumb,dtype=np.uint8)
        mask = create_bw_mask(image_array)
        dominant_colors = get_dominant_colors(image_array, mask)
        return dominant_colors

    def detect_keypress(self):
        while True:
            print("press 'q' for exit and 'r' for search")
            key = readchar.readkey()
            if key == "q":
                break
            if key == "w":
                self.index = self.index - 1
                if self.index < 0:
                    self.index = len(self.songs) - 1
                self.print_search()
            if key == "s":
                self.index = self.index + 1
                if self.index > len(self.songs) - 1:
                    self.index = 0
                self.print_search()
            if key == "r":
                self.search()
            else:
                try:
                    if ord(key) == 10:
                        self.type = self.songs[self.index]['resultType']
                        if self.type == 'song':
                            color = self.color_extract()
                            self.url = "https://www.youtube.com/watch?v=" + self.songs[self.index]['videoId']
                            usart_send(color)
                            webbrowser.open(self.url, new=0, autoraise=False)
                        elif self.type == 'album':
                            color = self.color_extract()
                            usart_send(color)
                            self.url = "https://www.youtube.com/playlist?=" + "&list=" + self.songs[self.index]['playlistId'] + "&index=1"
                            webbrowser.open(self.url, new=0, autoraise=False)


                        else:
                            print("unknown type: ",self.songs[self.index]['type'])
                except:
                    pass


m1 = music()
m1.search()
m1.detect_keypress()


