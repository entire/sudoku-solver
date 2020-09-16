//
// Created by kosuke on 9/16/20.
//

#ifndef SUDOKU_RECOGNIZER_H
#define SUDOKU_RECOGNIZER_H

namespace Sudoku {

class Recognizer
{
private:
    /* data */
public:
    Recognizer();
    Recognizer(const Recognizer&) = delete;
    ~Recognizer();
    void Setup();
};

} // namespace Sudoku

#endif //SUDOKU_RECOGNIZER_H
