/*
 * $ID: $
 */
/*
 * Demonstration of simple server
 */

/*
 * Modified by Masao Takaku.
 * For Japan/MARC server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include <backend.h>
#include <xmalloc.h>
#include <proto.h>
#include <log.h>

#include "jmarcserver.h"

int bend_sort (void *handle, bend_sortrequest *req, bend_sortresult *res)
{
    res->errcode = 1;
    res->errstring = "Sort not implemented";
    res->sort_status = Z_SortStatus_failure;
    return 0;
}

bend_initresult *bend_init(bend_initrequest *q)
{
    bend_initresult *r = odr_malloc (q->stream, sizeof(*r));
    static char *dummy = "Hej fister";

    r->errcode = 0;
    r->errstring = 0;
    r->handle = dummy;
    q->bend_sort = bend_sort;    /* register sort handler */
    return r;
}

/*
 * Z_Term をもらって文字列を返す。
 */
void get_term(char *buf, Z_Term *term, bend_searchresult *r)
{
  buf[0] = '\0';
  
  switch (term->which) {
  case Z_Term_general:
    memcpy(buf, term->u.general->buf, term->u.general->len);
    buf[term->u.general->len] = '\0';
    return;
    break;
  case Z_Term_numeric:
    logf(LOG_FATAL, "Numeric Term Not Support.");
    r->errcode = 229;
    r->errstring = "Numeric Term Not support.";
    return;
    break;
  case Z_Term_characterString:
    logf(LOG_FATAL, "Character String Not Support.");
    r->errcode = 229;
    r->errstring = "Character String Not support.";
    return;
    break;
  case Z_Term_oid:
    logf(LOG_FATAL, "OID Term Not Support.");
    r->errcode = 229;
    r->errstring = "OID Term Not Support.";
    return;
    break;
  case Z_Term_dateTime:
    logf(LOG_FATAL, "Date-Time Term Not Support.");
    r->errcode = 229;
    r->errstring = "Date-Time Term Not Support.";
    return;
    break;
  case Z_Term_external:
    logf(LOG_FATAL, "External Term Not Support.");
    r->errcode = 229;
    r->errstring = "External Term Not Support.";
    return;
    break;
  case Z_Term_integerAndUnit:
    logf(LOG_FATAL, "Integer & Unit Term Not Support.");
    r->errcode = 229;
    r->errstring = "Integer & Unit Term Not Support.";
    return;
    break;
  case Z_Term_null:
    logf(LOG_FATAL, "NULL Term Not Support.");
    r->errcode = 229;
    r->errstring = "NULL Term Not Support.";
    return;
    break;
  default:
    logf(LOG_FATAL, "Unknown Term");
    r->errcode = 229;
    r->errstring = "Unknown Term";
    return;
    break;
  }
}

/*
 * Z_Operandを文字列に変換して返す。
 * まだgeneral以外には未対応。
 */
void get_operand(char *term, Z_Operand *op, bend_searchresult *r)
{
  int i = 0;
  char buf[BUFSIZE];
  char buf_term[BUFSIZE];
  Z_AttributeElement *element;

  term[0]='\0';

  switch (op->which) {
  case Z_Operand_APT:
    if (! op->u.attributesPlusTerm->num_attributes) {
      get_term(term, op->u.attributesPlusTerm->term, r);
      return;
    }
    /* AttributeListを見る */
    while (i < op->u.attributesPlusTerm->num_attributes) {
      element = op->u.attributesPlusTerm->attributeList[i];
      switch (*element->attributeType) {
      case 1:
	switch (element->which) {
	case Z_AttributeValue_numeric:
	  switch (*element->value.numeric) {
	  case 1016: /* 全部 (Any) */
	    get_term(buf_term, op->u.attributesPlusTerm->term, r);
	    strcat(term, buf_term);
	    logf(LOG_DEBUG, "This is Any search");
	    break;
	  case 4:    /* 書名 (Title) */
	    get_term(buf_term, op->u.attributesPlusTerm->term, r);
	    sprintf(buf, "+title:%s", buf_term);
	    strcat(term, buf);
	    logf(LOG_DEBUG, "This is Title-field search");
	    break;
	  case 1003: /* 著者名 (Author) */
	    get_term(buf_term, op->u.attributesPlusTerm->term, r);
	    sprintf(buf, "+author:%s", buf_term, buf_term);
	    strcat(term, buf);
	    logf(LOG_DEBUG, "This is Author-field search");
	    break;
	  case 7:    /* ISBN */
	    get_term(buf_term, op->u.attributesPlusTerm->term, r);
	    sprintf(buf, "+isbn:%s", buf_term);
	    strcat(term, buf);
	    logf(LOG_DEBUG, "This is ISBN-field search");
	    break;
	  case 21:   /* 件名 (Subject Headings) */
	    get_term(buf_term, op->u.attributesPlusTerm->term, r);
	    sprintf(buf, "+subject:%s", buf_term, buf_term);
	    strcat(term, buf);
	    logf(LOG_DEBUG, "This is SubjectHeadings-field search");
	    break;
	  case 1018: /* 出版者 (Publisher) */
	    get_term(buf_term, op->u.attributesPlusTerm->term, r);
	    sprintf(buf, "+publisher:%s", buf_term, buf_term);
	    strcat(term, buf);
	    logf(LOG_DEBUG, "This is Publisher-field search");
	    break;
	  case 63:  /* 注記 (Note) */
	    get_term(buf_term, op->u.attributesPlusTerm->term, r);
	    sprintf(buf, "+note:%s", buf_term, buf_term);
	    strcat(term, buf);
	    logf(LOG_DEBUG, "This is Note-field search");
	    break;
	  default: /* Non-support */
	    logf(LOG_LOG, "This is Non-Support-field search");
	    r->errcode = 114;
	    sprintf(r->errstring, "%d", *element->value.numeric);
	    break;
	  }
	  break;
	case Z_AttributeValue_complex:
	  logf(LOG_LOG, "This is Complex AttributesValue");
	  r->errcode = 246;
	  break;
	default:
	  r->errcode = 108;
	  logf(LOG_DEBUG, "Unknown AttributeValue");
	  break;
	}
	break;
      }
      i++;
    }
    break;
  case Z_Operand_resultSetId:
    logf(LOG_FATAL, "ResultSetID Not Support.");
    r->errcode = 18;
    r->errstring = "ResultSetID Not Support.";
    break;
  case Z_Operand_resultAttr:
    logf(LOG_FATAL, "ResultSetPlusAttributes Not Support.");
    r->errcode = 245;
    r->errstring = "ResultSetPlusAttributes Not Support.";
    break;
  default:
    logf(LOG_FATAL, "Unknown Operand");
    r->errcode = 3;
    r->errstring = "Unknown Operand";
    break;
  }
}

/*
 * Z_Operatorを文字列に変換する。
 */
void get_operator(char *query, Z_Operator *op)
{
  switch(op->which) {
  case Z_Operator_and:
    strcpy(query, "and");
    break;
  case Z_Operator_or:
    strcpy(query, "or");
    break;
  case Z_Operator_and_not:
    strcpy(query, "not");
    break;
  case Z_Operator_prox:
    logf(LOG_FATAL, "Proximity operator not support");
    query = NULL;
    break;
  default:
    logf(LOG_FATAL, "Unknown operator");
    query = NULL;
    break;
  }
}

/*
 * parse_rpn_structure() 関数
 *
 * 説明: RPNを解析して,結果集合のQueryを含むかを判定する。
 * 返値: (int)
 * 	1   : 結果集合を含む。
 * 	0   : 結果集合を含まない。
 * 引数:
 * 	(Z_RPNStructure *) rpn : 解析すべきRPNの検索式
 */
int parse_rpn_structure(Z_RPNStructure *rpn)
{
  switch(rpn->which) {
  case Z_RPNStructure_simple:
    if (rpn->u.simple->which == Z_Operand_APT) {
      return 0;
    } else {
      return 1;
    }
    break;
  case Z_RPNStructure_complex:
    if (parse_rpn_structure(rpn->u.complex->s1)) {
      return 1;
    } else {
      return parse_rpn_structure(rpn->u.complex->s2);
    }
    break;
  default:     /* don't go to here from anywhere. */
    break;
  }
}

/*
 * rpn_to_namazu_query() 関数
 *
 * 説明: RPNの検索式を解析して,Namazu用の検索式に変換する。   
 * 返値: (void)
 * 引数:
 * 	(Z_RPNStructure *)rpn : 解析すべき検索式。
 *	(char *) query : Namazu用の検索文字列。
 *	(bend_searchresult *) r : Z39.50での検索結果(エラー処理用)
 */
void rpn_to_namazu_query(Z_RPNStructure *rpn, char *query, bend_searchresult *r)
{
  char left_query[BUFSIZE], right_query[BUFSIZE], operator[BUFSIZE];

  switch (rpn->which) {
  case Z_RPNStructure_complex:
    rpn_to_namazu_query(rpn->u.complex->s1, left_query, r);
    
    rpn_to_namazu_query(rpn->u.complex->s2, right_query, r);

    get_operator(operator, rpn->u.complex->roperator);

    sprintf(query, "( %s %s %s )", left_query, operator, right_query);
    break;
  case Z_RPNStructure_simple:
    get_operand(query, rpn->u.simple, r);
    break;
  default:
    strcpy(query, "!!!UNKNOWN-RPN!!!");
    logf(LOG_FATAL, "Unknown RPN Structure");
    break;
  }
}

/*
 * 名前: longvowel_to_hyphen() 関数
 *
 * 説明: query中の長音（ー）を全てハイフン（−）に置換する。
 *	 ※ EUC以外のコードが来ると結果は不定なので注意！
 *	 Japan/MARCデータがもともと半角カナで記述されているため。
 *	（いわゆる「インタ−ネット」問題への措置）
 * 返値: (void)
 * 引数:
 *	char *query : 検索式
 */
void longvowel_to_hyphen(char *query)
{
  int i, len = strlen(query);

  for (i = 0; i < len; i++) {
    if ((*(query+i) == '\xa1') && (*(query+i+1) == '\xbc')) {
      *(query+i) = '\xa1';
      *(query+i+1) = '\xdd';
      i++;
    }
  }
}

/*
 * 名前: rpn_to_resultset() 関数
 *
 * 説明: RPNを階層的に検索して結果集合を求める。
 * 返値: (void)
 * 引数:
 *	Z_RPNStructure *rpn : RPN
 *	char *resultfile: 結果集合ファイル
 * 	bend_searchresult *r : (エラー処理用)
 */
void rpn_to_resultset(Z_RPNStructure *rpn, char *resultfile, bend_searchresult *r)
{
  char cmd_string[BUFSIZE];
  char operator[BUFSIZE];
  char query[BUFSIZE], left_query[BUFSIZE], right_query[BUFSIZE];
  char left_resultfile[BUFSIZE], right_resultfile[BUFSIZE];

  switch (rpn->which) {
  case Z_RPNStructure_complex:

    get_operator(operator, rpn->u.complex->roperator);
    
    if (parse_rpn_structure(rpn)) {
      sprintf(left_resultfile, "%s.l", resultfile);
      rpn_to_resultset(rpn->u.complex->s1, left_resultfile, r);
      sprintf(right_resultfile, "%s.r", resultfile);
      rpn_to_resultset(rpn->u.complex->s2, right_resultfile, r);
      if (!strcmp(operator, "and")) {
	sprintf(cmd_string, "sort %s %s | uniq -d > %s",
		left_resultfile, right_resultfile, resultfile);
      } else if (!strcmp(operator, "or")) {
	sprintf(cmd_string, "sort %s %s | uniq > %s",
		left_resultfile, right_resultfile, resultfile);
      } else if (!strcmp(operator, "not")) {
	sprintf(cmd_string, "sort %s %s %s | uniq -u > %s",
		left_resultfile, right_resultfile, right_resultfile, resultfile);
      }
      system(cmd_string);
      logf(LOG_LOG, "%s", cmd_string);
    } else {
      rpn_to_namazu_query(rpn->u.complex->s1, left_query, r);
      rpn_to_namazu_query(rpn->u.complex->s2, right_query, r);
      sprintf(query, "( %s %s %s )", left_query, operator, right_query);
      longvowel_to_hyphen(query);
      sprintf(cmd_string,
	      "%s %s \"%s\" %s > %s",
	      NAMAZU_PATH, NAMAZU_OPTION, query, NAMAZU_DATABASE, resultfile);
      system(cmd_string);
      logf(LOG_LOG, "%s", cmd_string);
    }
    break;
  case Z_RPNStructure_simple:
    if (parse_rpn_structure(rpn)) {
      sprintf(cmd_string, "ln -s %s-%d.%s %s",
	     RESULTFILE, getpid(), rpn->u.simple->u.resultSetId, resultfile);
      system(cmd_string);
      logf(LOG_LOG, "%s", cmd_string);
    } else {
      get_operand(query, rpn->u.simple, r);
      longvowel_to_hyphen(query);
      sprintf(cmd_string,
	      "%s %s \"%s\" %s > %s",
	      NAMAZU_PATH, NAMAZU_OPTION, query, NAMAZU_DATABASE, resultfile);
      system(cmd_string);
      logf(LOG_LOG, "%s", cmd_string);
    }
    break;
  default:
    logf(LOG_FATAL, "Unknown RPN Structure");
    break;
  }
}

/*
 * 名前: process_rpn_query()
 *
 * 説明: Type1 & Type101 Query (RPN query) を処理する。
 * 返値: (void)
 * 引数:
 *	bend_searchresult *r : Z39.50の検索結果。
 *	Z_RPNQuery *rpn_query : RPN Query
 *	char *setname : 現在の結果集合の名前
 */
void process_rpn_query(bend_searchresult *r, Z_RPNQuery *rpn_query, char *setname)
{
  char buf[BUFSIZE];
  char resultfile[BUFSIZE];
  int line_num;
  FILE *fp;

  sprintf(resultfile, "%s-%d.%s", RESULTFILE, getpid(), setname);
  rpn_to_resultset(rpn_query->RPNStructure, resultfile, r);
  
  /* ヒット数のカウント */
  line_num = 0;
  if ((fp = fopen(resultfile, "r")) == NULL ) {
    r->errcode=1;
    logf(LOG_FATAL, "%s cannot open", resultfile);
    return;
  }
  while (fgets(buf, BUFSIZE, fp) != NULL) {
    line_num++;
  }
  r->hits = line_num;
  logf(LOG_LOG, "%d Hits", r->hits);
  fclose(fp);
}

bend_searchresult *bend_search(void *handle, bend_searchrequest *q, int *fd)
{
    bend_searchresult *r = odr_malloc (q->stream, sizeof(*r));

    r->errcode = 0;
    switch ( q->query->which ) {
    case Z_Query_type_1:
    case Z_Query_type_101:
      process_rpn_query(r, q->query->u.type_1, q->setname);
      break;
    case Z_Query_type_2:
      logf(LOG_FATAL, "Type-2 Query Not Support.");
      r->errcode = 107;
      r->errstring = "Type-2 Query Not Support.";
      break;
    default:
      logf(LOG_FATAL, "Unknown Query");
      r->errcode = 107;
      r->errstring = "Unknown Query";
      break;
    }

    return r;
}

static int atoin (const char *buf, int n)
{
    int val = 0;
    while (--n >= 0)
    {
        if (isdigit(*buf))
            val = val*10 + (*buf - '0');
        buf++;
    }
    return val;
}

char *marc_read(FILE *inf)
{
    char length_str[5];
    size_t size;
    char *buf;

    if (fread (length_str, 1, 5, inf) != 5)
        return NULL;
    size = atoin (length_str, 5);
    if (size <= 6)
        return NULL;
    if (!(buf = xmalloc (size+1)))
        return NULL;
    if (fread (buf+5, 1, size-5, inf) != (size-5))
    {
        xfree (buf);
        return NULL;
    }
    memcpy (buf, length_str, 5);
    buf[size] = '\0';
    return buf;
}

bend_fetchresult *bend_fetch(void *handle, bend_fetchrequest *q, int *num)
{
  bend_fetchresult *r = odr_malloc (q->stream, sizeof(*r));
  static char *bbb = 0;
  char buf[BUFSIZE];
  char recordfile[BUFSIZE];
  char record[BUFSIZE * 10];
  char resultfilename[BUFSIZE];
  FILE *fp;
  int i;

  r->errcode = 0;
  r->errstring = 0;
  r->basename = DBNAME;
  r->last_in_set = 0;
  if (bbb) {
    xfree(bbb);
    bbb = 0;
  }

  if (q->format != VAL_SUTRS) {
      r->errcode = 238;
      r->errstring = "SUTRS";
      logf(LOG_DEBUG, "%s : alternative-suggested syntax=SUTRS", diagbib1_str(238));
      return r;
  } else {
      r->format = q->format;
  }

  /* 結果集合のファイルリスト */
  sprintf(resultfilename, "%s-%d.%s", RESULTFILE, getpid(), q->setname);
  if ((fp = fopen(resultfilename, "r")) == NULL) {
    logf(LOG_FATAL, "cannot open %s", resultfilename);
    r->errcode=30;
    sprintf(r->errstring, "%s", q->setname);
    return r;
  }
  for ( i = 0; i < q->number; i++) {
    fgets(buf, BUFSIZE, fp);
  }
  fclose(fp);
  
  /* 実際のレコードファイル */
  strncpy(recordfile, buf, strlen(buf)-1);
  recordfile[strlen(buf)-1] = '\0';
  if ((fp = fopen(recordfile, "r")) == NULL) {
    logf(LOG_FATAL, "cannot open %s", recordfile);
    r->errcode=14;
    sprintf(r->errstring, "%s.%d", q->setname, q->number);
    return r;
  }
  record[0] = '\0';
  while (fgets(buf, BUFSIZE, fp) != NULL ) {
    strcat(record, buf);
  }
  fclose(fp);

  assert(r->record = bbb = xmalloc(strlen(record)+1));
  strcpy(bbb, record);
  r->len = strlen(record);

  return r;
}

bend_deleteresult *bend_delete(void *handle, bend_deleterequest *q, int *num)
{
    return 0;
}

/*
 * silly dummy-scan what reads words from a file.
 */
bend_scanresult *bend_scan(void *handle, bend_scanrequest *q, int *num)
{
    bend_scanresult *r = odr_malloc (q->stream, sizeof(*r));
    static FILE *f = 0;
    static struct scan_entry list[200];
    static char entries[200][80];
    int hits[200];
    char term[80], *p;
    int i, pos;

    r->errstring = 0;
    r->entries = list;
    r->status = BEND_SCAN_SUCCESS;
    if (!f && !(f = fopen("dummy-words", "r")))
    {
	perror("dummy-words");
	exit(1);
    }
    if (q->term->term->which != Z_Term_general)
    {
    	r->errcode = 229; /* unsupported term type */
	return r;
    }
    if (q->term->term->u.general->len >= 80)
    {
    	r->errcode = 11; /* term too long */
	return r;
    }
    if (q->num_entries > 200)
    {
    	r->errcode = 31;
	return r;
    }
    memcpy(term, q->term->term->u.general->buf, q->term->term->u.general->len);
    term[q->term->term->u.general->len] = '\0';
    for (p = term; *p; p++)
    	if (islower(*p))
	    *p = toupper(*p);

    fseek(f, 0, 0);
    r->num_entries = 0;
    for (i = 0, pos = 0; fscanf(f, " %79[^:]:%d", entries[pos], &hits[pos]) == 2;
	i++, pos < 199 ? pos++ : (pos = 0))
    {
    	if (!r->num_entries && strcmp(entries[pos], term) >= 0) /* s-point fnd */
	{
	    if ((r->term_position = q->term_position) > i + 1)
	    {
	    	r->term_position = i + 1;
		r->status = BEND_SCAN_PARTIAL;
	    }
	    for (; r->num_entries < r->term_position; r->num_entries++)
	    {
	    	int po;

		po = pos - r->term_position + r->num_entries + 1; /* find pos */
		if (po < 0)
		    po += 200;
		list[r->num_entries].term = entries[po];
		list[r->num_entries].occurrences = hits[po];
	    }
	}
	else if (r->num_entries)
	{
	    list[r->num_entries].term = entries[pos];
	    list[r->num_entries].occurrences = hits[pos];
	    r->num_entries++;
	}
	if (r->num_entries >= q->num_entries)
	    break;
    }
    if (feof(f))
    	r->status = BEND_SCAN_PARTIAL;
    return r;
}

void bend_close(void *handle)
{
    char rm_resultset[BUFSIZE];

    sprintf(rm_resultset, "rm -f %s-%d.*", RESULTFILE, getpid());
    system(rm_resultset);
    return;
}

int main(int argc, char **argv)
{
    return statserv_main(argc, argv);
}
