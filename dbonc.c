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

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
  PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum { 
  STATEMENT_INSERT, STATEMENT_SELECT
} StatementType;

typedef struct {
  StatementType type;
} Statement;

/*如果执行的是metacommand，就通过这个方法来处理*/
MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
  switch (statement->type) {
    case (STATEMENT_INSERT):
      printf("这里以insert开头，应该执行一个insert语句.\n");
      break;
    case (STATEMENT_SELECT):
      printf("这里以select开头，应该执行一个insert语句.\n");
      break;
  }
}

int main(int argc, char const *argv[])
{
  InputBuffer* input_buffer = new_input_buffer();
  // 2. 定义循环接收
  while (true) {
    print_prompt();
    read_input(input_buffer);

    // 把以点.开始的命令叫做 MetaCommand，直译为元命令语言，这里指除了SQL之外的语言
    if (input_buffer->buffer[0] == '.') {
      switch (do_meta_command(input_buffer)) {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("无效语句： '%s'\n", input_buffer->buffer);
          continue;
      }
    }
 
    Statement statement;
    switch (prepare_statement(input_buffer, &statement)) {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("无效语句 '%s'.\n",
               input_buffer->buffer);
        continue;
    }
 
    execute_statement(&statement);
  }
}
