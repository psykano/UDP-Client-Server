#include "EnetBase.h"

void EnetBase::kill() {
	adapter.enetDestroy();
	adapter.enetDeinit();
}

void EnetBase::sendQueuedPackets() {
	adapter.enetSendQueuedPackets();
}