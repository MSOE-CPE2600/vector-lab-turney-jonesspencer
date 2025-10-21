/**
 * @file parse.h
 * @brief Header for input parsing and command execution layer.
 *
 * Course: CPE2600
 * Section: 121
 * Assignment: Lab 5 — Vector Calculator
 * Name: Spencer Jones
 */
#ifndef PARSE_H
#define PARSE_H
#include "vector.h"

int parse_inputs();
int list_maxlength();
void init();
int list_resize();
void list_clear();
void cleanup();

#endif
