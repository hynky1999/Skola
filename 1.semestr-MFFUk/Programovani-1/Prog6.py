class lineReader():
    def __init__(self):
        self.l_buffer = ""

    def read(self,line):
        self.l_buffer += line

    def polishPhrase(self, phrase):
        if(len(phrase) == 0):
            return phrase
        if(phrase[0] == '\n' or phrase[0] == ' '):
            phrase = phrase[1:]
        phrase = phrase.replace('\n',' ')
        phrase += '.\n'
        return phrase

    def phrases(self):
        ph = self.l_buffer.split('.')
        self.l_buffer = ph[-1]
        ph = ph[0:-1]
        for i,p in enumerate(ph):
            ph[i] = self.polishPhrase(p)

        return ph


lR = lineReader()
with open('a.txt', 'r') as rf:
    with open('b.txt', 'w') as wf:
        for line in rf.readlines():
            lR.read(line)
            phrases = lR.phrases()
            if phrases:
                wf.writelines(phrases)
