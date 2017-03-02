/* Copyright (c) 2017 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include "font/font_manager.h"
#include "Support/Geometry.h"
#include "Node/Node.h"

NS_DOROTHY_BEGIN

class TrueTypeFile : public Object
{
public:
	PROPERTY_READONLY(bgfx::TrueTypeHandle, Handle);
	CREATE_FUNC(TrueTypeFile);
	virtual ~TrueTypeFile();
protected:
	TrueTypeFile(bgfx::TrueTypeHandle handle);
private:
	bgfx::TrueTypeHandle _handle;
};

class Font : public Object
{
public:
	PROPERTY_READONLY(bgfx::FontHandle, Handle);
	PROPERTY_READONLY_REF(bgfx::FontInfo, Info);
	CREATE_FUNC(Font);
	virtual ~Font();
protected:
	Font(TrueTypeFile* file, bgfx::FontHandle handle);
private:
	bgfx::FontHandle _handle;
	Ref<TrueTypeFile> _file;
};

class FontManager : public bgfx::FontManager
{
protected:
	FontManager() { }
	SINGLETON_REF(FontManager, FontCache);
};

#define SharedFontManager \
	Dorothy::Singleton<Dorothy::FontManager>::shared()

class SpriteEffect;
class Sprite;
class Texture2D;

class FontCache
{
public:
	PROPERTY_READONLY(SpriteEffect*, DefaultEffect);
	PROPERTY_READONLY(bgfx::FontManager*, Manager);
	virtual ~FontCache();
	void loadAync(String fontName, Uint32 fontSize, Uint32 fontIndex,
		const function<void(Font* font)>& callback);
	Font* load(String fontName, Uint32 fontSize, Uint32 fontIndex = 0);
	void unload();
	Sprite* createCharacter(Font* font, bgfx::CodePoint character);
	std::tuple<Texture2D*, Rect> getCharacterInfo(Font* font, bgfx::CodePoint character);
	const bgfx::GlyphInfo* getGlyphInfo(Font* font, bgfx::CodePoint character);
	const bgfx::GlyphInfo* updateCharacter(Sprite* sp, Font* font, bgfx::CodePoint character);
protected:
	FontCache();
private:
	Ref<SpriteEffect> _defaultEffect;
	unordered_map<string, Ref<TrueTypeFile>> _fontFiles;
	unordered_map<string, Ref<Font>> _fonts;
	SINGLETON_REF(FontCache, BGFXDora, FontManager);
};

#define SharedFontCache \
	Dorothy::Singleton<Dorothy::FontCache>::shared()

ENUM_START(TextAlignment)
{
	Left,
	Center,
	Right
}
ENUM_END(TextAlignment)

class Label : public Node
{
public:
	virtual ~Label();
	PROPERTY(TextAlignment, Alignment);
	PROPERTY(float, TextWidth);
	PROPERTY(float, LineGap);
	PROPERTY(const char*, Text);
	Sprite* getCharacter(int index) const;
	int getCharacterCount() const;
	static const float AutomaticWidth;
	CREATE_FUNC(Label);
protected:
	Label(String fontName, Uint32 fontSize);
	void updateCharacters(const vector<Uint32>& chars);
	void updateLabel();
	float getLetterPosXLeft(Sprite* sp);
	float getLetterPosXRight(Sprite* sp);
private:
	float _textWidth;
	float _lineGap;
	Ref<Font> _font;
	TextAlignment _alignment;
	string _textUTF8;
	vector<Uint32> _text;
	vector<Sprite*> _characters;
};

NS_DOROTHY_END