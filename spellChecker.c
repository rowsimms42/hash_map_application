//#pragma warning(disable : 4996).

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */

char* nextWord(FILE* file)
{
	int maxLength = 16;
	int length = 0;
	char* word = malloc(sizeof(char) * maxLength);
	while (1)
	{
		char c = fgetc(file);
		if ((c >= '0' && c <= '9') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			c == '\'')
		{
			if (length + 1 >= maxLength)
			{
				maxLength *= 2;
				word = realloc(word, maxLength);
			}
			word[length] = c;
			length++;
		}
		else if (length > 0 || c == EOF)
		{
			break;
		}
	}
	if (length == 0)
	{
		free(word);
		return NULL;
	}
	word[length] = '\0';
	return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
	// FIXME: implement
	char* x = nextWord(file);

	while (x != NULL)
	{
		hashMapPut(map, x, 1);
		free(x);
		x = nextWord(file);
	}

}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */

char* toLowerCase(char *word, int size)
{
	for (int i = 0; i < size; i++)
	{
		word[i] = tolower(word[i]);
	}
	return word;
}

//source used: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
int lev(char *s1, char *s2)
{
	unsigned int x, y, s1_length, s2_length;
	s1_length = strlen(s1);
	s2_length = strlen(s2);
	unsigned int arr[s2_length + 1][s1_length + 1];
	arr[0][0] = 0;

	for (x = 1; x <= s2_length; x++)
	{
		arr[x][0] = arr[x - 1][0] + 1;
	}
	for (y = 1; y <= s1_length; y++)
	{
		arr[0][y] = arr[0][y - 1] + 1;
	}
	for (x = 1; x <= s2_length; x++)
	{
		for (y = 1; y <= s1_length; y++)
		{
			arr[x][y] = MIN3(arr[x - 1][y] + 1, arr[x][y - 1] + 1, arr[x - 1][y - 1] + (s1[y - 1] == s2[x - 1] ? 0 : 1));
		}
	}
	return (arr[s2_length][s1_length]);
}


int main(int argc, const char** argv)
{
	// FIXME: implement
	HashMap* map = hashMapNew(1000);

	FILE* file = fopen("dictionary.txt", "r");
	clock_t timer = clock();
	loadDictionary(file, map);
	timer = clock() - timer;
	printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
	fclose(file);

	char inputBuffer[256];
	int size;
	int quit = 0;
	//int count = 0; //keeps track of the amount of word suggestions
	const int num = 5;
	char* wordMat[5];

	while (!quit)
	{
		printf("Enter a word or \"quit\" to quit: ");
		scanf("%s", inputBuffer);
		size = strlen(inputBuffer);
		toLowerCase(inputBuffer, size); //convert to all lowercase

		if (strcmp(inputBuffer, "quit") == 0)
		{
			quit = 1;
		}
		else
		{
			if (hashMapContainsKey(map, inputBuffer)) //dictionary contains word
			{
				printf("The inputted word %s ", inputBuffer);
				printf("is spelled correctly.\n");

			}
			else //dictionary does not contain word
			{
				printf("The inputted word %s ", inputBuffer);
				printf("is spelled incorrectly.\n");

				//array holds 5 matched words
				for (int i = 0; i < num; i++)
				{
					wordMat[i] = malloc(sizeof(char) * 24);
				}

				int x[5];
				for (int i = 0; i < 5; i++)
				{
					x[i] = 0;
				}

				HashLink* cur;

				for (int i = 0; i < map->capacity; i++)
				{
					cur = map->table[i];

					while (cur != NULL)
					{
						int compLev = lev(inputBuffer, cur->key);

						for (int i = 0; i < num; i++)
						{
							if (x[i] == 0 || x[i] > compLev)
							{
								x[i] = compLev;
								strcpy(wordMat[i], cur->key); //copy string
								break;
							}
						}

						cur = cur->next;
					}
				}


				//print
				printf("Did you mean.. : \n");
				for (int i = 0; i < num; i++)
				{
					printf("%s\n", wordMat[i]);
				}
				for (int i = 0; i < num; i++)
				{
					free(wordMat[i]);

				}

				//free(wordMat);
			}

		}
	}

	hashMapDelete(map);
	return 0;
}

