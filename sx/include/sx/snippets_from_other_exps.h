template<typename ITEM, typename MEMBER>
MEMBER sum(const vector <ITEM> &x, function<MEMBER(ITEM)> Pr) {
    MEMBER s;
    for (auto &q:x) s += Pr(q);
    return s;
}

template<typename MEMBER>
class select_member {
public:
    template<typename ITEM, MEMBER ITEM::*MEMBERPTR>
    void set(vector <ITEM> &items) {
        that = &items;
        stub_ptr = &stub < ITEM, MEMBERPTR >;
    }

    MEMBER operator[](int x) {
        return (*stub_ptr)(that, x);
    }

private:
    void *that;

    MEMBER (*stub_ptr)(void *, int);

    template<typename ITEM, MEMBER ITEM::*MEMBERPTR>
    static MEMBER stub(void *th, int x) {
        return (&(((vector <ITEM> *) th)->at(x)))->*MEMBERPTR;
    }
};

#define mk_field_va(CONT_VAR, MEMBER_N) mk_field_va_function<decltype(CONT_VAR), decltype(CONT_VAR[0].MEMBER_N)>(CONT_VAR, &decltype(CONT_VAR)::value_type::MEMBER_N)

template<typename CONT_T, typename MEMBER_T>
valarray <MEMBER_T> mk_field_va_function(const CONT_T &vc, MEMBER_T CONT_T::value_type::*MEMBER_PTR) {
    const size_t N = vc.size();
    valarray <MEMBER_T> va(N);
    for (size_t i = 0; i < N; ++i)
        va[i] = &vc[i]->*MEMBER_PTR;
    return va;
}

template<typename CONT_T, typename MEMBER_T>
class field_proxy {
public:
    field_proxy(CONT_T &vc, MEMBER_T CONT_T::value_type::*member_ptr) : vc(vc), member_ptr(member_ptr) {
    }

    MEMBER_T &operator[](size_t x) {
        return &vc[x]->*member_ptr;
    }

    const MEMBER_T &operator[](size_t x) const {
        return &vc[x]->*member_ptr;
    }

private:
    CONT_T &vc;
    MEMBER_T CONT_T::value_type::*member_ptr;
};

#define mk_field_proxy(CVAR, MNAME) field_proxy<decltype(CVAR), decltype(CVAR[0].MNAME)>(CVAR,&decltype(CVAR)::value_type::MNAME)

#define mk_rac_data_slice(RAC_VAR, DMEM_NAME) kobj_cont_data_member_proxy<decltype(RAC_VAR), decltype(RAC_VAR[0].DMEM_NAME)>(RAC_VAR,&decltype(RAC_VAR)::value_type::DMEM_NAME)
#define kobj_ptr_from_rac_data_slice(RAC_VAR, DMEM_NAME) kobj_ptr(new mk_rac_data_slice(RAC_VAR, DMEM_NAME))


cout << "Hello, World!" <<
endl;
vector <ize> izes(2);
izes[0].
a = 1;
izes[1].
a = 2;

auto va = mk_field_va(izes, a);

auto afp = mk_field_proxy(izes, a);


int fp0 = afp[0];
int fp1 = afp[1];

select_member<int> ai;
ai.
set<ize, &ize::a>(izes);

int a0 = ai[0];
int a1 = ai[1];


//select data member of vector of type

template<typename MEMBER>
class select_member {
public:
    template<typename ITEM, MEMBER ITEM::*MEMBERPTR>
    void set(vector <ITEM> &items) {
        that = &items;
        stub_ptr = &stub<ITEM, MEMBERPTR>;
    }

    MEMBER operator[](int x) {
        return (*stub_ptr)(that, x);
    }

private:
    void *that;

    MEMBER (*stub_ptr)(void *, int);

    template<typename ITEM, MEMBER ITEM::*MEMBERPTR>
    static MEMBER stub(void *th, int x) {
        return (&(((vector <ITEM> *) th)->at(x)))->*MEMBERPTR;
    }
};


//extract data member of vector of types into valarray
template<typename ITEM_T, typename MEMBER_T, MEMBER_T ITEM_T::*MEMBER_PTR>
valarray <MEMBER_T> va(vector <ITEM_T> &vc) {
    const size_t N = vc.size();
    valarray <MEMBER_T> va(N);
    for (size_t i = 0; i < N; ++i)
        va[i] = &vc[i]->*MEMBER_PTR;
    return va;
}
 