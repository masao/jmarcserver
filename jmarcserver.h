/* バッファサイズ */
#define BUFSIZE 10240

/* Namazuのindexのあるディレクトリ */
#define NAMAZU_DATABASE "/project/jmarc/"

/* 検索コマンドnamazuのパス */
#define NAMAZU_PATH 	"/home/masao/namazu/bin/namazu"
/* namazuに与えるオプション引数 */
#define NAMAZU_OPTION	"-uSa"

/* Search時の結果ファイルのprefix */
#define RESULTFILE "/home/masao/Z39.50/search-result"

/* Database Name */
#define DBNAME "JPMARC"

void process_rpn_query(bend_searchresult *r, Z_RPNQuery *rpn_query, char *setname);
void get_operand(char *term, Z_Operand *o, bend_searchresult *r);
void get_operator(char *query, Z_Operator *o);
int parse_rpn_structure(Z_RPNStructure *rpn);
void longvowel_to_hyphen(char *query);
void rpn_to_namazu_query(Z_RPNStructure *rpn, char *query, bend_searchresult *r);
void rpn_to_resultset(Z_RPNStructure *rpn, char *resultfile, bend_searchresult *r);
