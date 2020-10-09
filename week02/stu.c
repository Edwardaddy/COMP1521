int nswaps;
int last = ss->nstu;
do {
    nswapes = 0;
    for (int i = 1; i < last; i++) {
        if (strcmp(ss->recs[i].name, ss->recs[i-1].name) < 0) {
            struct _stu_rec tmp;
            tmp = ss->recs[i];
            ss->recs[i] = ss->recs[i-1];
            ss->recs[i-1] = tmp;
            nswaps++;
        }
    }
    last--

}while (nswaps > 0);