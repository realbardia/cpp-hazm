#include "hazm.h"

#include <Python.h>

#include <iostream>
#include <unordered_map>

using namespace std::string_literals;
using namespace Hazm;

PyObject *HazmObject::m_hazm_dict = nullptr;
PyObject *HazmObject::m_hazm_module = nullptr;
std::unordered_set<HazmObject*> HazmObject::m_hazm_objects;


HazmObject::HazmObject(const std::string &py_name, const std::vector<std::pair<std::string, std::string> > &kwargs)
{
    if (!m_hazm_module)
    {
        if (!Py_IsInitialized())
            Py_Initialize();

        auto pName = PyUnicode_FromString("hazm");
        m_hazm_module = PyImport_Import(pName);

        if (m_hazm_module)
            m_hazm_dict = PyModule_GetDict(m_hazm_module);
        else
            throw std::string("Hazm not found"s);
    }

    if (kwargs.size())
    {
        std::string kwargs_str;
        for (const auto &p: kwargs)
        {
            if (kwargs_str.size())
                kwargs_str += ',';
            kwargs_str = p.first + "='"s + p.second + "'"s;
        }
        m_object = PyRun_String((py_name + '(' + kwargs_str + ')').c_str(), Py_eval_input, m_hazm_dict, PyDict_New());
    }
    else
    {
        auto fnc = PyDict_GetItem(m_hazm_dict, PyUnicode_FromString(py_name.c_str()));
        m_object = PyObject_CallObject(fnc, NULL);
    }

    if (!m_object)
        throw std::string("Could not find "s) + py_name;

    m_hazm_objects.insert(this);
}

HazmPyObject* HazmObject::vectorToPyObject(const std::vector<std::string> &data)
{
    PyObject* tuple = PyTuple_New( data.size() );
    if (!tuple)
        throw std::string("Unable to allocate memory for Python tuple"s);
    for (unsigned int i = 0; i < data.size(); i++)
    {
        PyObject *num = PyUnicode_FromString(data.at(i).c_str());
        if (!num)
        {
            Py_DECREF(tuple);
            throw std::string("Unable to allocate memory for Python tuple"s);
        }
        PyTuple_SET_ITEM(tuple, i, num);
    }

    return tuple;
}

std::vector<std::string> HazmObject::pyObjectToVector(HazmPyObject* incoming)
{
    std::vector<std::string> res;
    if (PyTuple_Check(incoming))
    {
        for(Py_ssize_t i = 0; i < PyTuple_Size(incoming); i++)
        {
            PyObject *value = PyTuple_GetItem(incoming, i);
            res.push_back( PyUnicode_AsUTF8(value) );
        }
    }
    else if (PyList_Check(incoming))
    {
        for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++)
        {
            auto value = PyList_GetItem(incoming, i);
            res.push_back( PyUnicode_AsUTF8(value) );
        }
    }
    return res;
}

HazmObject::~HazmObject()
{
    if (m_object)
        PyObject_Free(m_object);

    m_hazm_objects.erase(this);
    if (m_hazm_objects.size() == 0)
    {
        PyObject_Free(m_hazm_dict);
        PyObject_Free(m_hazm_module);
        m_hazm_dict = nullptr;
        m_hazm_module = nullptr;
//        Py_Finalize();
    }
}




Normalizer::Normalizer()
    : HazmObject("Normalizer"s, {})
{
}

Normalizer::~Normalizer()
{
    if (m_normalize_method)
        PyObject_Free(m_normalize_method);
}

std::string Normalizer::normalize(const std::string &text)
{
    m_normalize_method = PyObject_GetAttrString(m_object, "normalize");
    if (!m_normalize_method)
        throw std::string("Could not load Normalizer::normalize"s);

    auto args = PyTuple_Pack(1, PyUnicode_FromString(text.c_str()));
    auto res = PyObject_CallObject(m_normalize_method, args);
    return PyUnicode_AsUTF8(res);
}



Stemmer::Stemmer()
    : HazmObject("Stemmer", {})
{
}

Stemmer::~Stemmer()
{
    if (m_stem_method)
        PyObject_Free(m_stem_method);
}

std::string Stemmer::stem(const std::string &text)
{
    m_stem_method = PyObject_GetAttrString(m_object, "stem");
    if (!m_stem_method)
        throw std::string("Could not load Stemmer::stem"s);

    auto args = PyTuple_Pack(1, PyUnicode_FromString(text.c_str()));
    auto res = PyObject_CallObject(m_stem_method, args);
    return PyUnicode_AsUTF8(res);
}



Lemmatizer::Lemmatizer()
    : HazmObject("Lemmatizer", {})
{
}

Lemmatizer::~Lemmatizer()
{
    if (m_lemmatize_method)
        PyObject_Free(m_lemmatize_method);
}

std::string Lemmatizer::lemmatize(const std::string &text)
{
    m_lemmatize_method = PyObject_GetAttrString(m_object, "lemmatize");
    if (!m_lemmatize_method)
        throw std::string("Could not load Lemmatizer::lemmatize"s);

    auto args = PyTuple_Pack(1, PyUnicode_FromString(text.c_str()));
    auto res = PyObject_CallObject(m_lemmatize_method, args);
    return PyUnicode_AsUTF8(res);
}



SentenceTokenizer::SentenceTokenizer()
    : HazmObject("SentenceTokenizer"s, {})
{
}

SentenceTokenizer::~SentenceTokenizer()
{
    if (m_tokenize_method)
        PyObject_Free(m_tokenize_method);
}

std::vector<std::string> SentenceTokenizer::tokenize(const std::string &text)
{
    m_tokenize_method = PyObject_GetAttrString(m_object, "tokenize");
    if (!m_tokenize_method)
        throw std::string("Could not load SentenceTokenizer::tokenize"s);

    auto args = PyTuple_Pack(1, PyUnicode_FromString(text.c_str()));
    auto res = PyObject_CallObject(m_tokenize_method, args);
    return pyObjectToVector(res);
}

WordTokenizer::WordTokenizer()
    : HazmObject("WordTokenizer"s, {})
{
}

WordTokenizer::~WordTokenizer()
{
    if (m_tokenize_method)
        PyObject_Free(m_tokenize_method);
}

std::vector<std::string> WordTokenizer::tokenize(const std::string &text)
{
    m_tokenize_method = PyObject_GetAttrString(m_object, "tokenize");
    if (!m_tokenize_method)
        throw std::string("Could not load WordTokenizer::tokenize"s);

    auto args = PyTuple_Pack(1, PyUnicode_FromString(text.c_str()));
    auto res = PyObject_CallObject(m_tokenize_method, args);
    return pyObjectToVector(res);
}



POSTagger::POSTagger(const std::string &model)
    : HazmObject("POSTagger"s, {{"model"s, model}})
{
}

POSTagger::~POSTagger()
{
    if (m_tag_method)
        PyObject_Free(m_tag_method);
}

std::vector<POSTagger::TagItem> POSTagger::pyObjectToTagsVector(HazmPyObject* incoming)
{
    std::vector<POSTagger::TagItem> res;
    if (PyTuple_Check(incoming))
    {
        for(Py_ssize_t i = 0; i < PyTuple_Size(incoming); i++)
        {
            auto value = PyTuple_GetItem(incoming, i);
            auto pair = pyObjectToVector(value);
            if (pair.size() != 2)
                continue;

            POSTagger::TagItem t;
            t.label = pair.at(0);
            t.word = pair.at(1);
            res.push_back(t);
        }
    }
    else if (PyList_Check(incoming))
    {
        for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++)
        {
            auto value = PyList_GetItem(incoming, i);
            auto pair = pyObjectToVector(value);
            if (pair.size() != 2)
                continue;

            POSTagger::TagItem t;
            t.label = pair.at(0);
            t.word = pair.at(1);
            res.push_back(t);
        }
    }
    return res;
}

HazmPyObject *POSTagger::pyObjectToTagsVector(const std::vector<TagItem> &data)
{
    PyObject* tuple = PyTuple_New( data.size() );
    if (!tuple)
        throw std::string("Unable to allocate memory for Python tuple"s);
    for (unsigned int i = 0; i < data.size(); i++)
    {
        const auto &t = data.at(i);
        PyObject *num = vectorToPyObject({t.label, t.word});
        if (!num)
        {
            Py_DECREF(tuple);
            throw std::string("Unable to allocate memory for Python tuple"s);
        }
        PyTuple_SET_ITEM(tuple, i, num);
    }

    return tuple;
}

std::vector<POSTagger::TagItem> POSTagger::tag(const std::vector<std::string> &tokens)
{
    m_tag_method = PyObject_GetAttrString(m_object, "tag");
    if (!m_tag_method)
        throw std::string("Could not load POSTagger::tokenize"s);

    auto args = PyTuple_Pack(1, vectorToPyObject(tokens));
    auto res = PyObject_CallObject(m_tag_method, args);
    return pyObjectToTagsVector(res);
}




Chunker::Chunker(const std::string &model)
    : HazmObject("Chunker"s, {{"model"s, model}})
{
}

Chunker::~Chunker()
{
    if (m_parse_method)
        PyObject_Free(m_parse_method);
}

Chunker::TreeNode Chunker::parse(const std::vector<POSTagger::TagItem> &tags)
{
    m_parse_method = PyObject_GetAttrString(m_object, "parse");
    if (!m_parse_method)
        throw std::string("Could not load Chunker::parse"s);

    auto args = PyTuple_Pack(1, POSTagger::pyObjectToTagsVector(tags));
    auto res = PyObject_CallObject(m_parse_method, args);
    return pyObjectToTreeNode(res);
}

Chunker::TreeNode Chunker::pyObjectToTreeNode(HazmPyObject *tree)
{
    auto label_method = PyObject_GetAttrString(tree, "label");
    if (!label_method)
        throw std::string("Could not load Tree::label"s);

    auto pop_method = PyObject_GetAttrString(tree, "pop");
    if (!pop_method)
        throw std::string("Could not load Tree::pop"s);

    auto label_py = PyObject_CallObject(label_method, NULL);

    Chunker::TreeNode node;
    node.label = PyUnicode_AsUTF8(label_py);

    auto child_obj = PyObject_CallObject(pop_method, NULL);
    while (child_obj)
    {
        if (PyTuple_Check(child_obj))
        {
            auto vector = pyObjectToVector(child_obj);

            Chunker::TreeNode c;
            c.label = vector.at(0);
            c.word = vector.at(1);
            node.childs.push_back(c);
        }
        else
        if (std::string(child_obj->ob_type->tp_name) == "Tree"s)
            node.childs.push_back( pyObjectToTreeNode(child_obj) );

        child_obj = PyObject_CallObject(pop_method, NULL);
    }

    return node;
}