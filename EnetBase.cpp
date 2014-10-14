#include "EnetBase.h"

void EnetBase::sendQueuedPackets() {
	adapter.enetSendQueuedPackets();
}

void EnetBase::kill() {
	adapter.enetDestroy();
	adapter.enetDeinit();
}