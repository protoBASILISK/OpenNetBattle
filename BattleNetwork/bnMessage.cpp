#include "bnMessage.h"
#include "bnAnimatedTextBox.h"
#include "bnTextureResourceManager.h"
Message::Message(std::string message) : MessageInterface(message) {
  nextCursor.setTexture(LOAD_TEXTURE(TEXT_BOX_NEXT_CURSOR));
  nextCursor.scale(2.0f, 2.0f);
  totalElapsed = 0;
}

Message::~Message() { ; }

void Message::OnUpdate(double elapsed) {
  totalElapsed += elapsed;
}

void Message::OnDraw(sf::RenderTarget& target, sf::RenderStates states) {
  auto bounce = std::sin((float)totalElapsed * 20.0f);

  auto textBoxBottom = (GetTextBox()->getPosition().y + GetTextBox()->GetFrameHeight() / 2.0f) / 2.0f;
  nextCursor.setPosition(sf::Vector2f(GetTextBox()->GetFrameWidth() - 15.0f, textBoxBottom + bounce) * 2.0f);

  GetTextBox()->DrawMessage(target, states);

  if (!GetTextBox()->IsPlaying() && (GetTextBox()->IsEndOfMessage() || GetTextBox()->HasMessage())) {
    target.draw(nextCursor, states);
  }
}
void Message::SetTextBox(AnimatedTextBox * textbox)
{
  MessageInterface::SetTextBox(textbox);
}

void Message::Continue() {
  if (GetTextBox()->IsPlaying() || !GetTextBox()->HasMessage()) return;

  if (GetTextBox()->IsEndOfMessage()) {
    GetTextBox()->DequeMessage();
  }
  else {
    GetTextBox()->ShowNextLines();
  }
}

