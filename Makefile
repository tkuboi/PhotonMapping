all:
	g++ -o tga main.cpp photonMapping.cpp Image.cpp
clean:
	rm -rf core* *.o *.gch $(ALL) junk*

