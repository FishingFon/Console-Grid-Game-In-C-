#include <stdio.h>
#include <stdlib.h>
int tileCount = -1;
int rows = 5;
int cols = 5;
void rotateMatrix(char** mat, int col, int rows, int numRotations)
{
	for(int k = 0; k < numRotations; k++){
		char temp = 'c';
		for (int i = 0; i < rows; i++)
		{
			for (int j = i + 1; j < col; j++)
			{
				temp = mat[i][j];
				mat[i][j] = mat[j][i];
				mat[j][i] = temp;
			}
		}

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < col / 2; j++)
			{
				temp = mat[i][j];
				mat[i][j] = mat[i][col - 1 - j];
				mat[i][col - 1 - j] = temp;
			}
		}
	}
}

void printTiles(char*** tiles){

	for(int i = 0; i < tileCount * 4; i+=4){
			
		for(int j = 0; j < rows; j++){
				
			printf("%s %s %s %s\n", tiles[i][j], tiles[i+1][j], tiles[i+2][j], tiles[i+3][j]);
		}

	printf("\n");
	}
}

void printTile(char*** tiles, int tileNumber){
	
	for(int j = 0; j < rows; j++){	
		printf("%s\n", tiles[tileNumber][j]);
	}

	printf("\n");
	
}

unsigned long fileSize(FILE* f)
{   
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
	rewind(f);
    return len;
}

char*** readTileFile(char* fileName){
	FILE* file = fopen(fileName, "r");
	if(file == NULL){
		fprintf(stderr, "Can’t access tile file\n");
		exit(2);
	}
	

    tileCount = fileSize(file)/(rows*cols);

	char*** tiles = (char***) malloc(sizeof(char**) * tileCount * 4);
	for(int i = 0; i < tileCount * 4; i++){
		tiles[i] = (char**) malloc(sizeof(char*) * rows);
		for(int j = 0; j < rows; j++){
			tiles[i][j] =  malloc(sizeof(char) * cols);
		}
	}
	
	
	char* line = (char*)malloc(70);
	int t = 0;
	int r = 0;
	int c = 0;
	while (1) {

        if (fgets(line,70, file) == NULL) {
			rotateMatrix(tiles[t+1], cols, rows, 1);
			rotateMatrix(tiles[t+2], cols, rows, 2);
			rotateMatrix(tiles[t+3], cols, rows, 3);
			break;
		}

		if(line[0] == '\n'){
			r = 0;
			c = 0;
			rotateMatrix(tiles[t+1], cols, rows, 1);
			rotateMatrix(tiles[t+2], cols, rows, 2);
			rotateMatrix(tiles[t+3], cols, rows, 3);


			t = t+4;
			continue;
		}
		
		for(int k = 0; line[k] != '\0'; ++k) {
			if(line[k] == '\n'){
				r++;
				break;
			}
			if((line[k] != ',' && line[k] != '!' ) || r > 4 || c > 4){
				fprintf(stderr, "Invalid tile file contents\n");
				exit(3);
			}
			
			tiles[t][r][c] = line[k];
			tiles[t+1][r][c] = line[k];
			tiles[t+2][r][c] = line[k];
			tiles[t+3][r][c] = line[k];
			c++;
		} 
		c = 0;
	}
	return tiles;
}


void loadSavedGame(){

}

char** createBoard(int height, int width){
	char** board = malloc(sizeof(char*) * height);
	for (int i = 0; i < height; i++) {
 		board[i] = malloc(sizeof(char) * width);
	}
	for(int j = 0; j < height; j++){
		for (int k = 0; k < width; ++k){
			board[j][k] = '.';
		}
	}

	return board;
}

void printBoard(char** board, int height){
	for(int j = 0; j < height; j++){
		printf("%s\n", board[j]);
	}	
}

int isMoveLegal(char** board, char** tile, int inputRow, int inputCol, int boardHeight, int boardWidth){
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if(tile[i][j] == '!'){
				int bx = i - 2 + inputRow;
				int by = j - 2 + inputCol;
				//printf("bx: %d, by: %d, i: %d, j: %d \n", bx, by, i, j);
				if(bx < 0 || bx >= boardWidth || by < 0 || by >= boardHeight){
					return 0;
				}
				if(board[bx][by] != '.'){
					return 0;		
				}


			}
		}
	}
	return 1;
}
void makeMove(char** board, char** tile, int inputRow, int inputCol, char playerType){
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if(tile[i][j] == '!'){
				int bx = i - 2 + inputRow;
				int by = j - 2 + inputCol;
				board[bx][by] = playerType;				
			}
		}
	}
}
int makeMoveIfLegal(char** board, char** tile, int inputRow, int inputCol, int boardHeight, int boardWidth, char playerType){
	if(isMoveLegal(board, tile, inputRow, inputCol, boardHeight, boardWidth)){
		makeMove(board, tile, inputRow, inputCol, playerType);
		return 1;
	}
	return 0;
}



int main(int argc, char** argv) {
	char*** tiles = NULL;
	int currentTile = 0;
	int playerOne = -1;
	int playerTwo = -1;
	int bHeight = -1;
	int bWidth = -1;
	//int players[2];
	
	char currentPlayer = '*';
	int cPlayer = 0;
	int players[2];
	char** board = NULL;
	

	if(argc == 2){
		tiles = readTileFile(argv[1]);

		printTiles(tiles);
		exit(0);
	}
	else if(argc == 5){
		tiles = readTileFile(argv[1]);

	}
	else if(argc == 6){
		tiles = readTileFile(argv[1]);
		

		if(!(argv[2][0] == 'h' || argv[2][0] == '1' || argv[2][0] == '2') 
		   || !(argv[3][0] == 'h' || argv[3][0] == '1' || argv[3][0] == '2')){
			fprintf(stderr, "Invalid player type\n");
			exit(4);		
		}

		if((atoi(argv[4]) < 1 || atoi(argv[4]) > 999)
		   || (atoi(argv[5]) < 1 || atoi(argv[5]) > 999)){
			fprintf(stderr, "Invalid dimensions\n");
			exit(5);	
		}
		playerOne = (argv[2]== "h")? 0 : atoi(argv[2]);
		playerTwo = (argv[3]== "h")? 0 : atoi(argv[3]);
		players[0] = playerOne;
		players[1] = playerTwo;

		bHeight = atoi(argv[4]);
		bWidth = atoi(argv[5]);
		//printf("p1: %d, p2: %d, wid: %d, hei: %d\n", playerOne, playerTwo, bHeight, bWidth);

		board = createBoard(bHeight, bWidth);
		/*
		printTile(tiles, 8);
		makeMoveIfLegal(board, tiles[8], -1, -1, bHeight, bWidth, '#');
		//int legal = isMoveLegal(board, tiles[0], 4, 4, bHeight, bWidth);
		printBoard(board, bHeight);
		*/
		printBoard(board, bHeight);		
		int inputRot = -1;
		int inputRow = -1000;
		int inputCol = -1000;
		char t1 = 0;
		char t2 = 0;
		char t3 = 0;
		char line[70];
		int it = 0;
		while(1){
			printTile(tiles, currentTile);
			printf("Player %c]", currentPlayer);
			fgets(line, sizeof(line), stdin);

			if(sscanf(line, "%d%c %d%c %d%c", &inputRow, &t1, &inputCol, &t2, &inputRot, &t3) != 6 || t1 != (int) 32 || t2 != (int) 32 || t3 != (int) 10){
				continue;
			}
			if(inputRot != 0 && inputRot != 90 && inputRot != 180 && inputRot != 270){
				continue;
			}
			if(makeMoveIfLegal(board, tiles[currentTile + (inputRot/90)], inputRow, inputCol, bHeight, bWidth, cPlayer? '#':'*')){
				cPlayer ?0:1;
				currentTile += 4;
				if(currentTile >= tileCount*4){
					currentTile = 0;
				}
				printBoard(board, bHeight);		

			}

		}

	}
	else{
		fprintf(stderr, "Usage: fitz tilefile [p1type p2type [height width | filename]]\n");
		exit(1);
	}


return 9;
}