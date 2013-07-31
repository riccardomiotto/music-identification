C++ implementation of music identification system

The main idea is to index a collection of songs by chroma words (i.e., hashed vectors 
of chroma features) and to use these words for retrieval of unknown songs (i.e., covers 
or different interpretations of a song in the collection, such as live and noisy 
distorted versions).

The code includes basic main functions for both indexing and retrieval:

- main/chromaIndexingMain.cpp

- main/chromaRetrievalMain.cpp

Both accept as input a directory containing the chroma files of the songs (i.e., textual 
files with N * 12 matrices, where N is the number of vectors extracted from the audio 
signal of the song), which need to be pre-computed. 

Retrieval is implemented as a binary search and is robust to tone changes 
(i.e., chroma transpositions).

The implementation is based on the algorithm described in:

Miotto, R. and Orio, N. (2008)
A Music Identification System Based on Chroma Indexing and Statistical Modeling
Proc. of the International Conference on Music Information Retrieval (ISMIR)
pp. 301-306. 

Please cite this paper if you use this code.

This version of the code was successfully compiled with the makefiles provided 
on mac os 10.8.4.