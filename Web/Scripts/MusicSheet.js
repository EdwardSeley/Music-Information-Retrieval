class MusicSheet {
    
    constructor(data)
    {
        this.data = data;
        this.startingPositionX = 20;
        this.startingPositionY = 100;
        this.canvas = null;
        this.width = 0;
        this.height = 0;
        this.c = null;
        this.notesInBar = 0;
        this.clefIsTreble = null;
    }
    
    drawSheet() {
        this.canvas = document.createElement('canvas');
        document.body.append(this.canvas);
        
        this.width = window.innerWidth
        || document.documentElement.clientWidth
        || document.body.clientWidth;

        this.height = window.innerHeight
        || document.documentElement.clientHeight
        || document.body.clientHeight;
        

        this.canvas.width = this.width;
        this.canvas.height = this.height;
        
        this.c = this.canvas.getContext('2d');

        this.drawStaff(this.startingPositionY);
        
        this.fillSheet();
    }
    
    clearSheet() {
        this.c.clearRect(0, 0, this.canvas.width, this.canvas.height);
        this.canvas.parentElement.removeChild(this.canvas);
        this.startingPositionX = 20;
        this.startingPositionY = 100;
        this.notesInBar = 0;
        this.clefIsTreble = null;
    }
    
    fillSheet() {
        this.data.forEach( (row) => {
            let columns = row.split(',');
            this.addNoteToStaff(columns[0], columns[1]);  
        })
        this.resize(this.width, this.startingPositionY+150);
    }

    resize(w, h) {
        let temp_canvas = document.createElement('canvas');
        let temp_c = temp_canvas.getContext('2d');
        temp_canvas.width = w;
        temp_canvas.height = h;
        temp_c.drawImage(this.canvas, 0, 0);
        this.canvas.width = w;
        this.canvas.height = h;
        this.c.drawImage(temp_canvas, 0, 0);
    }

    drawStaff(y) {
        const staffLineStartX = 15;
        const staffLineEndX = this.width;
        const staffLineSpace = 15;

        this.c.strokeStyle = "black";
        for (let i = 0; i < 5; i++) {
        this.c.beginPath();
        this.c.moveTo(staffLineStartX, y + staffLineSpace * i);
        this.c.lineTo(staffLineEndX, y + staffLineSpace * i);
        this.c.stroke();
        }
    }

    drawBar(x, y, length)
    {
        this.c.beginPath();
        this.c.moveTo(x, y);
        this.c.lineTo(x, y + length);
        this.c.stroke();
        this.startingPositionX += 70;
    }

    drawTreble(x, y) {
        let treble_image = new Image();
        treble_image.src = "trebleClef.SVG";
        treble_image.onload = () => {
            this.c.drawImage(treble_image, x, y, 41, 112);
        }
        this.startingPositionX += 90;
        this.clefIsTreble = true;
    }

    drawBass(x, y) {
        let bass_image = new Image();
        bass_image.src = "bassClef.SVG";
        bass_image.onload = () => {
            this.c.drawImage(bass_image, x, y, 41, 45);
        }
        this.startingPositionX += 90;
        this.clefIsTreble = false;
    }

    drawNote(x, y, sharp=false) {
        let note_image = new Image();
        note_image.src = "eighthNote.SVG";
        note_image.onload = () =>   {
            this.c.drawImage(note_image, x, y, 42, 78);
        }   
        if (sharp)  {
            let sharp_image = new Image();
            sharp_image.src = "sharp.SVG";
            sharp_image.onload = () =>   {
                this.c.drawImage(sharp_image, x-14, y + 60, 10, 24);
            }
        }
        this.startingPositionX += 70;
    }

    drawRest(x, y) {
        let rest_image = new Image();
        rest_image.src = "restNote.SVG";
        rest_image.onload = () =>   {
            this.c.drawImage(rest_image, x, y, 42, 78);
        }
        this.startingPositionX += 70;
    }

    getRelativeStaffPosition(note) {
        let staffPositions = {
            "B": -22.5, "A": -7.5, "G":0, "F":7.5, "E": 15, "D": 22.5, "C": 30,
        }
        let relativePosition = staffPositions[note[0]];
        if (note[1] == 2 || note[1] == 4)
            return relativePosition + 52.5;
        else
            return relativePosition;
    }

    addNoteToStaff(time, note) {
        if (note == undefined)
            return;

        if (this.startingPositionX + 80 > this.width) {
            this.resize(this.width, this.canvas.height + 250);
            this.startingPositionX = 20;
            this.startingPositionY += 250;
            this.drawStaff(this.startingPositionY);
        }

        if (note == "None") {
            this.drawRest(this.startingPositionX, this.startingPositionY-30);
        }
        else {
            let hasSharp = note.includes("#");
            if (hasSharp)
                note = note.replace("#", "");

            if (note[1] <= 3 && (this.clefIsTreble == null || this.clefIsTreble)) {
                if (this.notesInBar != 0)  {
                    this.drawBar(this.startingPositionX, this.startingPositionY, 60);
                    this.notesInBar = 0;
                }
                this.drawBass(this.startingPositionX, this.startingPositionY+2);
            }
            else if (note[1] > 3 && (this.clefIsTreble == null || !this.clefIsTreble)) {
                if (this.notesInBar != 0)  {
                    this.drawBar(this.startingPositionX, this.startingPositionY, 60);
                    this.notesInBar = 0;
                }
                this.drawTreble(this.startingPositionX, this.startingPositionY-30);
            }
            let posY = this.getRelativeStaffPosition(note);

            if (this.startingPositionX + 80 > this.width) {
                this.resize(this.width, this.canvas.height + 250);
                this.startingPositionX = 20;
                this.startingPositionY += 250;
                this.drawStaff(this.startingPositionY);
            }
            this.drawNote(this.startingPositionX, this.startingPositionY-78 + posY);
        }
        this.notesInBar++;

        if (this.notesInBar == 4)   {
            this.drawBar(this.startingPositionX, this.startingPositionY, 60);
            this.notesInBar = 0;
            this.c.font = "bold 16px Arial";
            this.c.fillText(String(time).substr(0, 4) + " s", this.startingPositionX-90, this.startingPositionY+85);
        }
    }

}