
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
  
  const uint32_t vector_bytes = 32;

  uint32_t input_rows;
  std::cout << "Input rows to generate: ";
  std::cin >> input_rows;
  std::cin.get();

  uint32_t columns = 1;
  std::cout << "Input columns to generate (vectorisation enabled if == 1): ";
  std::cin >> columns;
  std::cin.get();
  
  size_t column_stride = 0;
  if(columns > 1) {
    std::cout << "Input column offset stride: ";
    std::cin >> column_stride;
    std::cin.get();
  }

  // ensure rows is a multiple of vector_bytes
  const uint32_t rows = (input_rows / vector_bytes) * vector_bytes;

  const size_t column_bytes = 4, row_bytes = columns * column_bytes;


  uint16_t* offsets = new uint16_t[rows];
  uint8_t* data = new uint8_t[rows * row_bytes];
  if(offsets == 0 || data == 0) {
    std::cout << "Failed to allocate memory\n";
    return 1;
  }

  // generate some simple data
  std::cout << "Generating " << rows << " rows (aligned to " << vector_bytes << " byte vectorisation), " << columns << " columns\n";
  for(size_t i = 1; i < rows; ++i) {
    offsets[i] = (offsets[i-1]+column_stride) % columns;
  }

  for(size_t row_index = 0; row_index < rows; ++row_index) {
    for(size_t column_index = 0; column_index < columns; ++column_index) {
      uint32_t* value = reinterpret_cast<uint32_t*>(data + (row_index * row_bytes) + (column_index * column_bytes));
      *value = row_index * (column_index + 1);
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
    for(size_t row_index = 0; row_index < rows; ++row_index) {
      for(size_t column_index = 0; column_index < columns; ++column_index) {
        const uint32_t* value = reinterpret_cast<uint32_t*>(data + (row_index * row_bytes) + (column_index * column_bytes));
        std::cout << row_index << "," << column_index << ": " << *value << "\n";
      }
    }
  }


  uint32_t result;
  while(true) {
    std::cout << "\nNON-VECTORISED - press a key ";
    std::cin.get();

    result = 0;
    for(size_t row_index = 0; row_index < rows; ++row_index) {
      const uint16_t column_index = offsets[row_index];
      const uint32_t* value = reinterpret_cast<uint32_t*>(data + (row_index * row_bytes) + (column_index * column_bytes));
      result += *value;
    }

    std::cout << "result: " << result << "\n";


    // vectorized algorithm only works with contiguous values
    if(columns == 1) {
      std::cout << "\nVECTORISED - press a key ";
      std::cin.get();

      typedef uint32_t v4i __attribute__ ((vector_size (vector_bytes)));
      v4i vec_result = {0,0,0,0};

      for(size_t row_index = 0; row_index < rows; row_index += (vector_bytes / column_bytes)) {
        const v4i* vec_value = reinterpret_cast<v4i*>(data + (row_index * row_bytes));
        vec_result += *vec_value;
      }

      result = 0;
      uint32_t* vec_result_array = reinterpret_cast<uint32_t*>(&vec_result);
      for(size_t i = 0; i < (vector_bytes / column_bytes); ++i) {
        result += vec_result_array[i];
      }

      std::cout << "result: " << result << "\n";

    } else {
      std::cout << "columns != 1, skipping vectorised sum\n";
    }
  }
    
  return result;
}
