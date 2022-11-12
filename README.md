# cpp-hazm
C++ wrappers of the Python's [Hazm](https://github.com/sobhe/hazm) module

## Usage

```c++
#include "hazm.h"
    
using namespace Hazm;

int main()
{
    Normalizer normalizer;
    normalizer.normalize("اصلاح نويسه ها و استفاده از نیم‌فاصله پردازش را آسان مي كند");
    // 'اصلاح نویسه‌ها و استفاده از نیم‌فاصله پردازش را آسان می‌کند'

    sent_tokenize("ما هم برای وصل کردن آمدیم! ولی برای پردازش، جدا بهتر نیست؟");
    // ['ما هم برای وصل کردن آمدیم!', 'ولی برای پردازش، جدا بهتر نیست؟']
    
    word_tokenize("ولی برای پردازش، جدا بهتر نیست؟");
    // ['ولی', 'برای', 'پردازش', '،', 'جدا', 'بهتر', 'نیست', '؟']

    Stemmer stemmer;
    stemmer.stem("کتاب‌ها");
    // 'کتاب'
    
    Lemmatizer lemmatizer;
    lemmatizer.lemmatize("می‌روم");
    // 'رفت#رو'

    POSTagger tagger("resources/postagger.model");
    tagger.tag(word_tokenize("ما بسیار کتاب می‌خوانیم"));
    // [('ما', 'PRO'), ('بسیار', 'ADV'), ('کتاب', 'N'), ('می‌خوانیم', 'V')]

    Chunker chunker("resources/chunker.model");
    auto tagged = tagger.tag(word_tokenize("کتاب خواندن را دوست داریم"));
    tree2brackets(chunker.parse(tagged));
    // '[کتاب خواندن NP] [را POSTP] [دوست داریم VP]'
    
    return 0;
}
```


## Installation
You needs python-dev c++ and also Python's hazm module. You can install it using `pip`:

    pip install hazm

