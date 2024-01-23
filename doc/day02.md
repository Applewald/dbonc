## 环境
- Ubuntu 22.04.3 LTS
- cmake version 3.22.1
- GNU Make 4.3
- gcc version 11.4.0
- Visual Studio Code v1.83.1

环境安装不再赘述。

## REPL
read-eval-print loop，交互式循环解释器。我们用的命令行就是这个东西。
一个不明觉厉的概念而已，我们通过这个来让数据库可以接收SQL语句。
~~~c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 1. 首先定义InputBuffer来接收REPL中输入的语句
typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void print_prompt() {
  printf("db > ");
}

// 把读取到的数据给 InputBuffer
void read_input(InputBuffer* input_buffer) {
  /*getline() reads an entire line from stream, storing the address
       of the buffer containing the text into *lineptr.  The buffer is
       null-terminated and includes the newline character, if one was
       found.
    getline方法是一个libc的标准库，获取命令行中的数据流
    ssize_t getline(char **lineptr, size_t *n, FILE *stream);

    lineptr：该指针指向一个指向输入的内容的指针，说着好绕口。注意free。
    n：已分配缓冲区的大小。
    stream：输入流。
    返回值ssize_t：返回输入内容真正的大小，可能少于分配缓冲区的大小。
  */
  ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("读取命令错误\n");
    exit(EXIT_FAILURE);
  }

  // 去掉换行符
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

int main(int argc, char const *argv[])
{
  InputBuffer* input_buffer = new_input_buffer();
  // 2. 定义循环接收
  while (true) {
    print_prompt();
    read_input(input_buffer);

    // 如果以.exit结尾就关闭命令行
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
      close_input_buffer(input_buffer);
      exit(EXIT_SUCCESS);
    } else {
      printf("无效语句： '%s'.\n", input_buffer->buffer);
    }
  }
}
~~~