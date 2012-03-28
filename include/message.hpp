#pragma once

struct Message {
  boost::function<void*()> functionObject;
  Ticket * returnTicket;
};
