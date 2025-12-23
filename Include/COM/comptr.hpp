/*
 * Copyright (c) 2025 Weather
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TRACERAYER_COMPTR_HPP
#define TRACERAYER_COMPTR_HPP

template<typename T>
class ComRAII
{
public:
    ComRAII( const ComRAII& ) = delete;
    ComRAII& operator = ( const ComRAII& ) = delete;

    ComRAII( ComRAII&& other ) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }
    ComRAII& operator = ( ComRAII&& other ) noexcept
    {
        if (this != &other)
        {
            reset( other.ptr_ );
            other.ptr_ = nullptr;
        }
        return *this;
    }

    explicit ComRAII( T* p = nullptr ) : ptr_(p) {}

    ~ComRAII() noexcept
    {
        if (ptr_) ptr_->lpVtbl->Release(ptr_);
    }

    T* get() const { return ptr_; }

protected:
    T* ptr_ = nullptr;

    T** put() { reset(); return &ptr_; }

    void reset( T* p = nullptr ) noexcept
    {
        if ( ptr_ )
            ptr_->lpVtbl->Release(ptr_);
        ptr_ = p;
    }

    [[nodiscard]]
    T* detach() { T* t = ptr_; ptr_ = nullptr; return t; }
};

#endif