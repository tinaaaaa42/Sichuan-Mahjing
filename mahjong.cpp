#include <iostream>
#include <string>

#define CHICKEN_HAND 1
#define FULL_FLUSH 4
#define ALL_TRIPLETS 2
#define PURE_TRIPLETS 8
#define TRIPLETS258 8
#define TERMINALS_IN_ALL_SETS 4
#define PURE_TERMINALS 16
#define SEVEN_PAIRS 4
#define PURE_SEVEN_PAIRS 16

using namespace std;

class Mahjong {
	public:
		Mahjong(string s) {
			tiles = s;
			getTile(tiles);
			if (!ableToWin()) {
				return;
			}
			
			rootGeter();
			pure = isPure();
			if (isPair()) {
				if (pure) {
					score = PURE_SEVEN_PAIRS;
				} else {
					score = SEVEN_PAIRS;
				}
			} else {
				Solution solution(character, bamboo, dot, pure);
				score = solution.bestScore();
			}
			
		}
		
		/* @return  the final score */
		int getScore() {
			return score * root();	
		}
		
	private:
		/* store the tiles in private data
		   @param tiles  the input string (Hand & Draw) 
		*/
		void getTile(string tiles) {
			for (int i = tiles.size() - 1; i >= 0; i -= 1) {
				if (tiles[i] == 'c') {
					getCharacterHelper(i);
				}
				if (tiles[i] == 'b') {
					getBambooHelper(i);
				}
				if (tiles[i] == 'd') {
					getDotHelper(i);
				}
			}
		}
		
		/* help to get the ith character 
		   @param i  the index of the tiles
		   precondition: tiles[i] == 'c'
		*/
		void getCharacterHelper(int& i) {
			i -= 1;
			while (i >= 0 and tiles[i] > '0' and tiles[i] <= '9') {
				character[tiles[i] - '0'] += 1;
				i -= 1;
				suitCount[0] = true;
			}
			i += 1;
		}
		
		/* help to get the ith bamboo 
		   @param i  the index of the tiles
		   precondition: tiles[i] == 'b'
		*/
		void getBambooHelper(int& i) {
			i -= 1;
			while (i >= 0 and tiles[i] > '0' and tiles[i] <= '9') {
				bamboo[tiles[i] - '0'] += 1;
				i -= 1;
				suitCount[1] = true;
			}
			i += 1;
		}
		
		/* help to get the ith dot 
		   @param i  the index of the tiles
		   precondition: tiles[i] == 'd'
		*/
		void getDotHelper(int& i) {
			i -= 1;
			while (i >= 0 and tiles[i] > '0' and tiles[i] <= '9') {
				dot[tiles[i] - '0'] += 1;
				i -= 1;
				suitCount[2] = true;
			}
			i += 1;
		}
		
		/* scan the tiles to calculate the root */
		void rootGeter() {
			for (int i = 1; i <= 9; i += 1) {
				if (character[i] == 4) {
					rootInTiles *= 2;
				}
				if (bamboo[i] == 4) {
					rootInTiles *= 2;
				}
				if (dot[i] == 4) {
					rootInTiles *= 2;
				}
			}
		}
		
		/* @return  if #suit < 3, return true. Else, return false. */
		bool ableToWin() {
			return !(suitCount[0] and suitCount[1] and suitCount[2]);
		}
		
		/* @return  pow(2, #4-tiles) */
		int root() {
			return rootInTiles;
		}
		
		/* @return  if all tiles have even numbers, return true. Else, return false. */
		bool isPair() {
			for (int i = 1; i < 10; i += 1) {
				if (character[i] % 2 or bamboo[i] % 2 or dot[i] % 2) {
					return false;
				}
			}
			return true;
		}
		
		/* @return  if #suit == 1, return true. Else, return false. */
		bool isPure() {
			return (suitCount[0] and !suitCount[1] and !suitCount[2]) \
			        or (!suitCount[0] and suitCount[1] and !suitCount[2]) \
			        or (!suitCount[0] and !suitCount[1] and suitCount[2]);
		}
		
		class Solution {
			public:
				// only solve for tiles that don't apply for 7-pairs
				Solution(int *c, int *b, int *d, bool pure) {
					for (int i = 1; i < 10; i += 1) {
						data[i] = c[i];
						data[i + 10] = b[i];
						data[i + 20] = d[i];
					}
					isPure = pure;
					dataDealer();
					solve();
				}
				
				/* @return  the best score in all solutions (if exist) */
				int bestScore() {
					return maxScore;
				}
				
			private:
				void dataDealer() {
					isTriplets = tripletsChecker();
					number258 = numberChecker();
				}
				/* @return  if apply for Triplets, return true. Else, return false. */
				bool tripletsChecker() {
					int pairCount = 0;
					int index;
					for (int i = 1; i < 30; i += 1) {
						if (data[i] == 2) {
							pairCount += 1;
							index = i;
						}
					}
					if (pairCount == 1) {
						for (int j = 1; j < 30; j += 1) {
							if (j != index and data[j] != 0 and data[j] != 3) {
							    return false;
							}
						}
						return true;
					} else {
						return false;
					}
				}
				
				/* @return  if all tiles numbered 2,5 or 8, return true. Else, return false. */
				bool numberChecker() {
					for (int i = 1; i < 10; i += 1) {
						if ((data[i] != 0 or data[i + 10] != 0 or data[i + 20] != 0) \
						    	and i != 2 and i != 5 and i != 8) {
						    	return false;
							}
					}
					return true;
				}
				
				// the core algorithm
				void solve() {
					for (int i = 1; i < 30; i += 1) {
						if (data[i] > 1) {
							int tempData[30];
							copyData(data, tempData);
							tempData[i] -= 2;
							solveMelds(tempData, 12, true);
						}
					}
				}
				
				/* solve 3 * n tiles to find melds recursively 
				   @param currentData  the temp data working on
				   @param totalNumber  the current number of tiles to be considered 
				   @param isTerminals  whether the current approach is terminals 
				   (example: 111222333 has 2 different appoaches, also different for terminals)
				*/
				void solveMelds(int *currentData, int totalNumber, bool isTerminals) {
					if (totalNumber == 0) {
						if (isTerminals) {
							if (isPure) {
								updateScore(PURE_TERMINALS);
								// cout << "PURE_TERMINALS" << "\n";
							} else {
								updateScore(TERMINALS_IN_ALL_SETS);
								// cout << "TERMINALS_IN_ALL_SETS" << "\n";
							}
						}
						if (isTriplets) {
							if (number258) {
								updateScore(TRIPLETS258);
								// cout << "TRIPLETS258" << "\n";
							} else if(isPure) {
								updateScore(PURE_TRIPLETS);
								// cout << "PURE_TRIPLETS" << "\n";
							} else {
								updateScore(ALL_TRIPLETS);
								// cout << "ALL_TRIPLETS" << "\n";
							}
						}
						if (isPure) {
							updateScore(FULL_FLUSH);
							// cout << "FULL_FLUSH" << "\n";
						} else {
							updateScore(CHICKEN_HAND);
							// cout << "CHICKEN_HAND" << "\n";
						}
					} else {
						for (int i = 1; i < 30; i += 1) {
							if (currentData[i] > 0) {
								if (whetherSequence(currentData, i)) {
									int tempData[30];
									copyData(currentData, tempData);
									bool newTerminals = sequenceDealer(tempData, i);
									solveMelds(tempData, totalNumber - 3, isTerminals and newTerminals);
								}
								if (whetherTriplets(currentData, i)) {
									int tempData[30];
									copyData(currentData, tempData);
									bool newTerminals = tripletsDealer(tempData, i);
									solveMelds(tempData, totalNumber - 3, isTerminals and newTerminals);
								}
								if (!whetherSequence(currentData, i) and !whetherTriplets(currentData, i)) {
									return;
								}
							}
						}
					}
				}
				
				/* @param i  the current index 
				   @return  if the ith tiles can be sequenced, return true. Else, return false.
				   precondition: tempData[i] > 0
				*/
				bool whetherSequence(int *currentData, int i) {
					return (i + 2 < 30 and currentData[i + 1] > 0 and currentData[i + 2] > 0);
				}
				
				/* @param i  the current index
				   @return  if the sequence contains terminals, return true. Else, return false.
				   precondition: whetherSequence(i) == true 
				*/
				bool sequenceDealer(int *tempData, int i) {
					tempData[i] -= 1;
					tempData[i + 1] -= 1;
					tempData[i + 2] -= 1;
					
					return (i % 10 == 1 or (i + 2) % 10 == 9);
				}
				
				/* @param i  the current index 
				   @return  if the ith tiles can be triplets, return true. Else, return false.
				   precondition: tempData[i] > 0
				*/
				bool whetherTriplets(int *currentData, int i) {
					return currentData[i] >= 3;
				}
				
				/* @param i  the current index 
				   @return  if the sequence contains terminals, return true. Else, return false.
				   precondition: whetherTriplets(i) == true
				*/
				bool tripletsDealer(int *tempData, int i) {
					tempData[i] -= 3;
					return (i % 10 == 1 or i % 10 == 9);
				}
				
				/* copy the tiles from source to target */
				void copyData(int *source, int *target) {
					for (int i = 0; i < 30; i += 1) {
						target[i] = source[i];
					}
				}
				
				/* updates the maxScore (may not change) 
				   @param s  the possible new score 
				*/
				void updateScore(int s) {
					if (s > maxScore) {
						maxScore = s;
					}
				}
				
				/* a subtle way to save data:
				   characters in 1 - 9,
				   bamboos in 11 - 19,
				   dots in 21 - 29,
				   set data[0] = data[10] = data[20] = 0
				   guarantee the sequence doesn't involve 2 types
				*/
				int data[30] = {0};
				bool isPure;
				bool isTriplets;
				bool number258;
				int maxScore = 0;
		};

		// private data
		bool suitCount[3] = {false};
		bool pure;
		int score = 0;
		int rootInTiles = 1;
		int character[10] = {0};
		int bamboo[10] = {0};
		int dot[10] = {0};
		string tiles;
};
