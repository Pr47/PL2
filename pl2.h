#ifndef PLAPI_PL2_H
#define PLAPI_PL2_H

#define PLAPI

#define PL2_MAJOR 0
#define PL2_MINOR 1
#define PL2_PATCH 0
#define PL2_POSTFIX "alpha"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*** -------------------------- pl2_Slice -------------------------- ***/

typedef struct st_pl2_slice {
  const char *start;
  const char *end;
} pl2_Slice;

pl2_Slice pl2_slice(const char *start, const char *end);
pl2_Slice pl2_nullSlice(void);
pl2_Slice pl2_sliceFromCStr(const char *cStr);
char *pl2_unsafeIntoCStr(pl2_Slice slice);

_Bool pl2_sliceCmp(pl2_Slice s1, pl2_Slice s2);
_Bool pl2_sliceCmpCStr(pl2_Slice slice, const char *cStr);
size_t pl2_sliceLen(pl2_Slice slice);
_Bool pl2_isNullSlice(pl2_Slice slice);

/*** ------------------------- pl2_MString ------------------------- ***/

typedef uint32_t pl2_MString;
typedef struct st_m_context {
} pl2_MContext;

pl2_MContext *pl2_mContext(void);
void pl2_mContextFree(pl2_MContext *context);
pl2_MString pl2_mString(pl2_MContext *context, const char *string);
const char *pl2_getString(pl2_MContext *context, pl2_MString mstring);

/*** -------------------------- pl2_Error -------------------------- ***/

typedef struct st_pl2_error {
  void *extraData;
  uint16_t errorCode;
  uint16_t line;
  char reason[0];
} pl2_Error;

typedef enum e_pl2_error_code {
  PL2_ERR_NONE = 0, /* No Error */
  PL2_ERR_GENERAL = 1, /* General hard error */
  PL2_ERR_PARTBUF = 2, /* Command parts exceed internal part buffer */
  PL2_ERR_UNCLOSED_STR = 3, /* Unclosed string literal */
  PL2_ERR_UNCLOSED_BEGIN = 4, /* Unclosed ?begin block */
  PL2_ERR_EMPTY_CMD = 5, /* Empty command */
  PL2_ERR_SEMVER_PARSE = 6, /* SemVer parse error */
  PL2_ERR_UNKNOWN_QUES = 7, /* Unknown question mark command */
} pl2_ErrorCode;

pl2_Error *pl2_errorBuffer(size_t strBufferSize);

void pl2_fillError(pl2_Error *error,
                   uint16_t errorCode,
                   uint16_t line,
                   const char *reason,
                   void *extraData);

void pl2_errPrintf(pl2_Error *error,
                   uint16_t errorCode,
                   uint16_t line,
                   void *extraData,
                   const char *fmt,
                   ...);

_Bool pl2_isError(pl2_Error *error);

/*** ----------------------- pl2_SourceInfo  ----------------------- ***/

typedef struct st_pl2_source_info {
  const char *fileName;
  uint16_t line;
} pl2_SourceInfo;

pl2_SourceInfo pl2_sourceInfo(const char *fileName, uint16_t line);

/*** --------------------------- pl2_Cmd --------------------------- ***/

typedef struct st_pl2_cmd_part {
  pl2_Slice prefix;
  pl2_Slice body;
} pl2_CmdPart;

pl2_CmdPart pl2_cmdPart(pl2_Slice prefix, pl2_Slice body);

#define PL2_EMPTY_PART(part) \
  (pl2_isNullSlice((part)->prefix) && pl2_isNullSlice((part)->body))

typedef struct st_pl2_cmd {
  struct st_pl2_cmd *prev;
  struct st_pl2_cmd *next;

  void *extraData;
  pl2_CmdPart parts[0];

} pl2_Cmd;

pl2_Cmd *pl2_cmd(pl2_CmdPart *parts);
pl2_Cmd *pl2_cmd4(pl2_Cmd *prev,
                  pl2_Cmd *next,
                  void *extraData,
                  pl2_CmdPart *parts);

/*** ------------------------- pl2_Program ------------------------- ***/

typedef struct st_pl2_program {
  pl2_Slice language;
  pl2_Cmd *commands;
} pl2_Program;

void pl2_initProgram(pl2_Program *program);
pl2_Program pl2_parse(char *source, pl2_Error *error);

/*** -------------------------- pl2_Exec  -------------------------- ***/

typedef void (pl2_SInvokeCmdStub)(const char *strings[]);
typedef pl2_Cmd* (pl2_PCallCmdStub)(pl2_Program *program,
                                    void *context,
                                    pl2_Cmd *command,
                                    pl2_Error *error);

typedef void* (pl2_InitStub)(pl2_Error *error);
typedef void (pl2_AtexitStub)(void *context, pl2_Error *error);

typedef struct st_pl2_sinvoke_cmd {
  pl2_MString cmdName;
  _Bool deprecated;
  _Bool removed;
  pl2_SInvokeCmdStub *stub;
} pl2_SInvokeCmd;

typedef struct st_pl2_pcall_func {
  pl2_MString cmdName;
  _Bool deprecated;
  _Bool removed;
  pl2_PCallCmdStub *stub;
} pl2_PCallCmd;

#define PL2_EMPTY_FUNC(func) \
  ((func)->funcName == 0 \
    && (func)->stub == 0 \
    && (func)->deprecated == 0 \
    && (func)->removed == 0)

typedef struct st_pl2_context {
  pl2_Program program;
  pl2_MContext *mContext;

  pl2_Cmd *currentCommand;
  pl2_SInvokeCmd *sinvokeFuncs;
  pl2_PCallCmd *pcallFuncs;
  
  pl2_InitStub *init;
  pl2_AtexitStub *atexit;
  pl2_PCallCmdStub *fallback;
  void *context;
} pl2_Context;

void pl2_initContext(pl2_Context *context,
                     pl2_Slice langName,
                     pl2_Error *error);
void pl2_run(pl2_Context *program, pl2_Error *error);
void pl2_clearContext(pl2_Context *context);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PLAPI_PL2_H */
