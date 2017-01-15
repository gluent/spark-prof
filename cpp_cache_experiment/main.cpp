
#include <iostream>


// http://burtleburtle.net/bob/rand/smallprng.html
typedef unsigned long int  u4;
typedef struct ranctx { u4 a; u4 b; u4 c; u4 d; } ranctx;

#define rot(x,k) (((x)<<(k))|((x)>>(32-(k))))
u4 ranval( ranctx *x ) {
  u4 e = x->a - rot(x->b, 27);
  x->a = x->b ^ rot(x->c, 17);
  x->b = x->c + x->d;
  x->c = x->d + e;
  x->d = e + x->a;
  return x->d;
}

void raninit( ranctx *x, u4 seed ) {
  u4 i;
  x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
  for (i=0; i<20; ++i) {
    (void)ranval(x);
  }
}



int main(int argc, char** argv) {

  const bool print_data = false;
  
  // set columns to 1 to show max potential vectorisation
  const uint32_t rows = 1000000, columns = 10;
  const size_t column_bytes = 4, row_bytes = columns * column_bytes;

  uint16_t* offsets = new uint16_t[rows];
  uint8_t* data = new uint8_t[rows * columns * column_bytes];
  if(offsets == 0 || data == 0) {
    std::cout << "Failed to allocate memory\n";
    return 1;
  }


  // generate some simple data
  const size_t column_stride = 1;
  for(size_t i = 1; i < rows; ++i) {
    offsets[i] = (offsets[i-1]+column_stride) % columns;
  }

  for(size_t row = 0; row < rows; ++row) {
    for(size_t column = 0; column < columns; ++column) {
      uint32_t* value = reinterpret_cast<uint32_t*>(data + (row * row_bytes) + (column * column_bytes));
      *value = row * (column + 1);
    }
  }


  if(print_data) {
    // display offsets
    std::cout << "offsets:\n";
    for(size_t i = 0; i < rows; ++i) {
      std::cout << offsets[i] << "\n";
    } 

    // display data
    std::cout << "\ndata:\n";
    for(size_t row = 0; row < rows; ++row) {
      for(size_t column = 0; column < columns; ++column) {
        const uint32_t* value = reinterpret_cast<uint32_t*>(data + (row * row_bytes) + (column * column_bytes));
        std::cout << row << "," << column << ": " << *value << "\n";
      }
    }
  }


  while(true) {
    std::cout << "\n\npress a key to continue\n";
    std::cin.get();

    uint32_t result = 0;
    for(size_t row = 0; row < rows; ++row) {
      const uint16_t column_index = offsets[row];
      const uint32_t* value = reinterpret_cast<uint32_t*>(data + (row * row_bytes) + (column_index * column_bytes));
      result += *value;
    }

    std::cout << "result: " << result << "\n";
  }

  return 0;
}
