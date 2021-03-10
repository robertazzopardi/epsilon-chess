/**
 * @file main.c
 * @author Robert Azzopardi-Yashi (robertazzopardi@icloud.com)
 * @brief
 * @version 0.1
 * @date 2021-03-10
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "common.h"

int main(int argc, char **argv)
{
	Board board = makeBoard();

	initialise(board);

	return 0;
}
