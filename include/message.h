#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "data.h"
#include "entry.h"


/* Definicao do numero de digitos do opcode e do c_type */
#define OPCODE_SIZE 2
#define C_TYPE_SIZE 2

/* Define os possíveis opcodes da mensagem, ou seja, operações que o cliente pode
 * realizar sobre o servidor, e o opcode de erro emitido pelo servidor em caso da
 * operação enviada pelo cliente seja mal sucedida.
*/
#define OP_SIZE		10
#define OP_DEL	   	20
#define OP_GET		30
#define OP_PUT		40
#define OP_GETKEYS	50
/* opcode para representar retorno de erro da execução da operação*/
#define OP_ERROR		99

/* Define códigos para os possíveis conteúdos da mensagem (c_type) */
#define CT_KEY    10
#define CT_VALUE  20
#define CT_ENTRY  30
#define CT_KEYS   40
#define CT_RESULT 50
/* Opcode representativo de inexistência de content (e.g., getkeys e size */
#define CT_NONE   60  


/* Estrutura que representa uma mensagem genérica a ser transmitida.
 * Esta mensagem pode ter vários tipos de conteúdos. */
struct message_t {
	short opcode;    /* código da operação na mensagem */
	short c_type;    /* tipo do conteúdo da mensagem */
	union content_u {
		struct data_t *value;
		char *key;
		struct entry_t *entry;
		char **keys;
		int result;
	} content;    /* conteúdo da mensagem */
};


/* Serializa o conteúdo de mensagem passada em message_t, colocando a
 * mensagem serializada como uma sequência de bytes num buffer cujo espaço
 * deverá ser alocado e cujo endereço deverá ser colocado no char* apontado
 * por msg_buf. Retorna o tamanho do buffer alocado, ou -1 em caso de erro.
 * A mensagem serializada numa sequência de bytes tem o seguinte formato:
 *
 *  OPCODE      C_TYPE      CONTENT
 * [2 bytes]   [2 bytes]   [CT bytes]
 *  
 * em que OPCODE corresponde ao código da operação (opcode) a realizar na
 * tabela do servidor, C_TYPE corresponde ao tipo de conteúdo da mensagem
 * e CONTENT corresponde ao conteúdo da mensagem.
 *
 * CONTENT depende de C_TYPE, da seguinte forma: 
 *
 * CT_KEY:        KEYSIZE(KS)     KEY
 *                 [2 bytes]   [KS bytes]
 *
 * CT_VALUE:      DATASIZE(DS)    DATA
 *                 [4 bytes]   [DS bytes]
 *
 * CT_ENTRY:      KEYSIZE(KS)     KEY      DATASIZE(DS)     DATA
 *                 [2 bytes]   [KS bytes]   [4 bytes]    [DS bytes]
 *
 * CT_KEYS:         NKEYS    KEYSZ_1     KEY_1    ...  KEYSZ_N     KEY_N
 *                [4 bytes] [2 bytes] [KS1 bytes] ... [2 bytes] [KSN bytes]
 *
 * CT_RESULT:      RESULT
 *                [4 bytes]
 *
 * Notar que o `\0´ no fim das strings (chaves) e o NULL no fim do array de
 * chaves não são enviados nas mensagens.
 *
 * Notar também que o bloco de dados binários correspondentes a DATA deve
 * ser codificado no formato BASE64. Para tal recomenda-se o uso da
 * biblioteca base64.h e base64.c (disponível em 
 * http://josefsson.org/base-encoding/)
 */
int message_to_buffer(struct message_t *msg, char **msg_buf);

/* De-serializa a mensagem contida em msg_buf, com tamanho msg_size,
 * colocando-a e retornando-a numa struct message_t, cujo espaço em
 * memória deve ser reservado.
 */
struct message_t *buffer_to_message(char *msg_buf, int msg_size);

/* Liberta a memoria alocada na função buffer_to_message para a
 * struct message_t.
 */
void free_message(struct message_t *msg);

#endif