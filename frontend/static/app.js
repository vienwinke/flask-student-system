// ===== 工具函数 =====
function qs(id) { return document.getElementById(id); }
function esc(str) {
    var d = document.createElement("div");
    d.textContent = str;
    return d.innerHTML;
}

function toast(msg, type) {
    var c = document.querySelector(".toast-container");
    if (!c) { c = document.createElement("div"); c.className = "toast-container"; document.body.appendChild(c); }
    var t = document.createElement("div");
    t.className = "toast toast-" + (type || "success");
    t.textContent = msg;
    c.appendChild(t);
    setTimeout(function() { t.style.opacity = "0"; t.style.transition = "opacity 0.3s"; setTimeout(function() { t.remove(); }, 300); }, 2500);
}

function getStatus(f) { return f >= 60 ? '<span class="badge badge-pass">及格</span>' : '<span class="badge badge-fail">不及格</span>'; }

document.addEventListener("click", function(e) {
    var btn = e.target.closest("[data-action]");
    if (!btn) return;
    var action = btn.getAttribute("data-action");
    if (action === "showStudent") showStudent(btn.getAttribute("data-sid"));
    else if (action === "deleteStudent") deleteStudent(btn.getAttribute("data-sid"));
    else if (action === "deleteScore") deleteScore(btn.getAttribute("data-sid"), btn.getAttribute("data-subject"));
    else if (action === "deleteWeight") deleteWeight(btn.getAttribute("data-subject"));
    else if (action === "editStudent") editStudent(btn.getAttribute("data-sid"), btn.getAttribute("data-sname"));
    else if (action === "editScore") editScore(btn.getAttribute("data-sid"), btn.getAttribute("data-subject"), btn.getAttribute("data-usual"), btn.getAttribute("data-end"));
});
var _submitting = false;
document.addEventListener("keydown", function(e) { if (e.key === "Escape") closeAddModal(); });


// 修改成绩
function editScore(sid, subject, usual, end) {
    qs('editScoreSid').value = sid;
    qs('editScoreSubject').value = subject;
    qs('editScoreUsual').value = usual;
    qs('editScoreEnd').value = end;
    qs('editScoreResult').style.display = 'none';
    qs('editScoreModal').classList.add('show');
}
function doEditScore() {
    var sid = qs('editScoreSid').value;
    var subject = qs('editScoreSubject').value;
    var usual = parseInt(qs('editScoreUsual').value);
    var end = parseInt(qs('editScoreEnd').value);
    if (isNaN(usual) || isNaN(end)) { toast('请输入有效分数', 'error'); return; }
    fetch('/api/scores/update', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({sid: sid, subject: subject, usual: usual, end: end}) })
    .then(function(r) { return r.json(); })
    .then(function(j) {
        if (j.error) { toast(j.error, 'error'); return; }
        var r = qs('editScoreResult');
        r.style.display = 'block';
        r.innerHTML = '修改成功！新总分 = ' + j.final_score + ' (权重 a=' + j.weight + ')';
        setTimeout(function() {
            closeAddModal();
            loadScores(); loadStats();
            if (typeof loadStudents === 'function') loadStudents();
        }, 1000);
    })
    .catch(function() { toast('网络错误', 'error'); });
}

function closeAddModal() { document.querySelectorAll(".modal-overlay").forEach(function(m) { m.classList.remove("show"); }); }

// ===== 仪表盘 =====
async function loadStats() {
    try {
        var r = await fetch("/api/stats");
        var d = await r.json();
        qs("statStudents").textContent = d.student_count;
        qs("statScores").textContent = d.score_count;
        qs("statSubjects").textContent = d.subject_count;
        qs("statWeights").textContent = d.weight_count;
        var tb = qs("top5Body");
        if (d.top5.length === 0) {
            tb.innerHTML = '<tr><td colspan="3"><div class="empty-state"><i class="bi bi-trophy"></i>暂无数据</div></td></tr>';
        } else {
            var h = "";
            for (var i = 0; i < d.top5.length; i++) {
                h += "<tr><td>" + esc(d.top5[i].name) + "</td><td>" + esc(d.top5[i].subject) + "</td><td><strong>" + d.top5[i].score + "</strong></td></tr>";
            }
            tb.innerHTML = h;
        }
    } catch (e) { console.error(e); }
}
if (qs("statsGrid")) loadStats();



// ===== Excel导入 =====
function showImportExcel() {
    qs('importModal').classList.add('show');
    var r = qs('importResult');
    r.style.display = 'none';
    r.className = '';
    qs('importFile').value = '';
    qs('importBtn').disabled = false;
}

function doImportExcel() {
    var fileInput = qs('importFile');
    if (!fileInput.files || fileInput.files.length === 0) {
        toast('请选择Excel文件', 'error');
        return;
    }
    var file = fileInput.files[0];
    if (!file.name.match(/\.xlsx?$/)) {
        toast('仅支持.xlsx或.xls文件', 'error');
        return;
    }

    var btn = qs('importBtn');
    btn.disabled = true;
    btn.innerHTML = '<i class="bi bi-hourglass-split"></i> 导入中...';

    var fd = new FormData();
    fd.append('file', file);

    fetch('/api/import/excel', { method: 'POST', body: fd })
    .then(function(r) { return r.json(); })
    .then(function(d) {
        btn.disabled = false;
        btn.innerHTML = '<i class="bi bi-upload"></i> 开始导入';

        var r = qs('importResult');
        r.style.display = 'block';
        
        if (d.error) {
            r.className = 'error';
            r.innerHTML = '<strong>导入失败：</strong>' + d.error;
            return;
        }

        var msg = '';
        if (d.students_added > 0) msg += '<div>新增学生：' + d.students_added + ' 人</div>';
        if (d.scores_added > 0) msg += '<div>新增成绩：' + d.scores_added + ' 条</div>';
        if (d.errors && d.errors.length > 0) {
            msg += '<div style="color:#991b1b;margin-top:4px">' + d.errors.slice(0, 5).join('<br>') + '</div>';
            if (d.errors.length > 5) msg += '<div>...还有 ' + (d.errors.length - 5) + ' 条错误</div>';
        }
        if (!d.students_added && !d.scores_added && (!d.errors || d.errors.length === 0)) {
            msg = '<div>未检测到可导入的数据。请确认列名包含"学号+姓名"或"学号+科目+平时分+期末分"。</div>';
        }

        r.className = (d.errors && d.errors.length > 0) ? 'error' : 'success';
        r.innerHTML = '<strong>导入完成</strong>' + msg;

        // Refresh data
        loadStudents(); loadStats();
        if (typeof loadScores === 'function') loadScores();
    })
    .catch(function() {
        btn.disabled = false;
        btn.innerHTML = '<i class="bi bi-upload"></i> 开始导入';
        var r = qs('importResult');
        r.style.display = 'block';
        r.className = 'error';
        r.innerHTML = '<strong>导入失败：</strong>网络错误';
    });
}

// ===== 学生管理 =====
var studentsData = [];

async function loadStudents() {
    try {
        var r = await fetch("/api/students");
        studentsData = await r.json();
        renderStudents(studentsData);
    } catch (e) { console.error(e); }
}

function renderStudents(data) {
    var tb = qs("studentBody");
    if (data.length === 0) {
        tb.innerHTML = '<tr><td colspan="6"><div class="empty-state"><i class="bi bi-people"></i>暂无学生数据</div></td></tr>';
        return;
    }
    var h = "";
    for (var i = 0; i < data.length; i++) {
        var s = data[i];
        h += '<tr><td><strong>' + esc(s.sid) + '</strong></td>' +
            '<td>' + esc(s.sname) + '</td>' +
            '<td>' + s.course_count + '</td>' +
            '<td>' + (s.avg_score > 0 ? s.avg_score : "-") + '</td>' +
            '<td><button class="btn-link" data-action="showStudent" data-sid="' + esc(s.sid) + '">查看详情</button>&nbsp;<button class="btn-link" data-action="editStudent" data-sid="' + esc(s.sid) + '" data-sname="' + esc(s.sname) + '"><i class="bi bi-pencil"></i></button></td>' +
            '<td><button class="btn-danger-outline" data-action="deleteStudent" data-sid="' + esc(s.sid) + '"><i class="bi bi-trash"></i></button></td>' +
            "</tr>";
    }
    tb.innerHTML = h;
}

function filterStudents() {
    var q = qs("studentSearch").value.trim().toLowerCase();
    if (!q) { renderStudents(studentsData); return; }
    var f = [];
    for (var i = 0; i < studentsData.length; i++) {
        var s = studentsData[i];
        if (s.sid.toLowerCase().indexOf(q) !== -1 || s.sname.toLowerCase().indexOf(q) !== -1) f.push(s);
    }
    renderStudents(f);
}

function showAddStudent() { qs("addStudentModal").classList.add("show"); }
function doAddStudent() {
    if (_submitting) return;
    _submitting = true;
    var f = qs("addStudentForm");
    var sid = f.querySelector("[name=sid]").value.trim();
    var sname = f.querySelector("[name=sname]").value.trim();
    if (!sid || !sname) { _submitting = false; return; }
    fetch("/api/students", { method: "POST", headers: {"Content-Type": "application/json"}, body: JSON.stringify({ sid: sid, sname: sname }) })
    .then(function(r) { return r.json(); })
    .then(function(j) {
        _submitting = false;
        if (j.error) { toast(j.error, "error"); return; }
        toast("学生 " + sname + " 添加成功");
        closeAddModal(); f.querySelector("[name=sid]").value = ""; f.querySelector("[name=sname]").value = "";
        loadStudents(); loadStats();
    })
    .catch(function() { _submitting = false; toast("网络错误", "error"); });
}


async function deleteStudent(sid) {
    if (!confirm("确定删除学生 " + sid + " 和他/她的全部成绩吗？")) return;
    try {
        var r = await fetch("/api/students/" + encodeURIComponent(sid), { method: "DELETE" });
        var j = await r.json();
        if (!r.ok) { toast(j.error || "删除失败", "error"); return; }
        toast("已删除学生 " + sid);
        loadStudents(); loadStats();
    } catch (e) { toast("网络错误", "error"); }
}

async function showStudent(sid) {
    try {
        var r = await fetch("/api/students/" + encodeURIComponent(sid));
        if (!r.ok) { toast("学生不存在", "error"); return; }
        var d = await r.json();
        qs("modalTitle").textContent = d.sname + " 的成绩详情";
        var h = '<div class="detail-grid"><div class="detail-item"><span class="label">学号</span><span class="value">' + esc(d.sid) + '</span></div><div class="detail-item"><span class="label">姓名</span><span class="value">' + esc(d.sname) + '</span></div></div>';
        if (d.scores.length === 0) {
            h += '<div class="empty-state"><i class="bi bi-journal-text"></i>暂无成绩记录</div>';
        } else {
            h += '<table class="sub-table"><thead><tr><th>科目</th><th>平时分</th><th>期末分</th><th>总分</th><th>状态</th></tr></thead><tbody>';
            for (var i = 0; i < d.scores.length; i++) {
                var sc = d.scores[i];
                h += "<tr><td>" + esc(sc.subject) + "</td><td>" + sc.usual + "</td><td>" + sc.end + "</td><td><strong>" + sc.final + "</strong></td><td>" + getStatus(sc.final) + "</td></tr>";
            }
            h += "</tbody></table>";
        }
        qs("modalBody").innerHTML = h;
        qs("studentModal").classList.add("show");
    } catch (e) { toast("加载失败", "error"); }
}

// 修改姓名
function editStudent(sid, sname) {
    qs('editSid').value = sid;
    qs('editSname').value = sname;
    qs('editStudentModal').classList.add('show');
}
function doEditStudent() {
    var sid = qs('editSid').value;
    var sname = qs('editSname').value.trim();
    if (!sname) { toast('姓名不能为空', 'error'); return; }
    fetch('/api/students/' + encodeURIComponent(sid), { method: 'PUT', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({sname: sname}) })
    .then(function(r) { return r.json(); })
    .then(function(j) {
        if (j.error) { toast(j.error, 'error'); return; }
        toast('姓名修改成功');
        closeAddModal(); loadStudents();
    })
    .catch(function() { toast('网络错误', 'error'); });
}

function closeModal() { qs("studentModal").classList.remove("show"); }
if (qs("studentTable")) loadStudents();

// ===== 成绩列表 =====
var scoresData = [];

async function loadScores() {
    try {
        var r = await fetch("/api/scores");
        scoresData = await r.json();
        renderScores(scoresData);
    } catch (e) { console.error(e); }
}

function renderScores(data) {
    var tb = qs("scoreBody");
    if (data.length === 0) {
        tb.innerHTML = '<tr><td colspan="8"><div class="empty-state"><i class="bi bi-table"></i>暂无成绩数据</div></td></tr>';
        return;
    }
    var h = "";
    for (var i = 0; i < data.length; i++) {
        var s = data[i];
        h += '<tr><td><strong>' + esc(s.sid) + '</strong></td>' +
            '<td>' + esc(s.sname) + '</td>' +
            '<td>' + esc(s.subject) + '</td>' +
            '<td>' + s.usual + '</td>' +
            '<td>' + s.end + '</td>' +
            '<td><strong>' + s.final + '</strong></td>' +
            '<td>' + getStatus(s.final) + '</td>' +
            '<td><button class="btn-link" data-action="editScore" data-sid="' + esc(s.sid) + '" data-subject="' + esc(s.subject) + '" data-usual="' + s.usual + '" data-end="' + s.end + '"><i class="bi bi-pencil"></i> 编辑</button>&nbsp;<button class="btn-danger-outline" data-action="deleteScore" data-sid="' + esc(s.sid) + '" data-subject="' + esc(s.subject) + '"><i class="bi bi-trash"></i></button></td>' +
            "</tr>";
    }
    tb.innerHTML = h;
}

function filterScores() {
    var q = qs("scoreSearch").value.trim().toLowerCase();
    if (!q) { renderScores(scoresData); return; }
    var f = [];
    for (var i = 0; i < scoresData.length; i++) {
        var s = scoresData[i];
        if (s.sid.toLowerCase().indexOf(q) !== -1 || s.sname.toLowerCase().indexOf(q) !== -1 || s.subject.toLowerCase().indexOf(q) !== -1) f.push(s);
    }
    renderScores(f);
}

function showAddScore() {
    fetch("/api/students").then(function(r) { return r.json(); }).then(function(data) {
        var dl = qs("studentList");
        var h = "";
        for (var i = 0; i < data.length; i++) { h += '<option value="' + esc(data[i].sid) + '">' + esc(data[i].sname) + '</option>'; }
        dl.innerHTML = h;
    });
    fetch("/api/weights").then(function(r) { return r.json(); }).then(function(data) {
        var dl = qs("subjectList");
        var h = "";
        for (var i = 0; i < data.length; i++) { h += '<option value="' + esc(data[i].subject) + '">权重 ' + data[i].weight + '</option>'; }
        dl.innerHTML = h;
    });
    qs("addScoreModal").classList.add("show");
}
function doAddScore() {
    if (_submitting) return;
    _submitting = true;
    var f = qs("addScoreForm");
    var sid = f.querySelector("[name=sid]").value.trim();
    var subject = f.querySelector("[name=subject]").value.trim();
    var usual = f.querySelector("[name=usual]").value;
    var end = f.querySelector("[name=end]").value;
    if (!sid || !subject) { _submitting = false; return; }
    fetch("/api/scores", { method: "POST", headers: {"Content-Type": "application/json"}, body: JSON.stringify({ sid: sid, subject: subject, usual: usual, end: end }) })
    .then(function(r) { return r.json(); })
    .then(function(j) {
        _submitting = false;
        if (j.error) { toast(j.error, "error"); return; }
        toast(subject + " 成绩=" + j.final_score + " (权重 a=" + j.weight + ")");
        closeAddModal(); f.querySelector("[name=sid]").value = ""; f.querySelector("[name=subject]").value = ""; f.querySelector("[name=usual]").value = "80"; f.querySelector("[name=end]").value = "80";
        loadScores(); loadStats(); if (typeof loadStudents === "function") loadStudents();
    })
    .catch(function() { _submitting = false; toast("网络错误", "error"); });
}


async function deleteScore(sid, subject) {
    if (!confirm("确定删除 " + sid + " 的 " + subject + " 成绩吗？")) return;
    try {
        var r = await fetch("/api/scores/delete", { method: "POST", headers: {"Content-Type": "application/json"}, body: JSON.stringify({ sid: sid, subject: subject }) });
        var j = await r.json();
        if (!r.ok) { toast(j.error || "删除失败", "error"); return; }
        toast("已删除成绩"); loadScores(); loadStats(); if (typeof loadStudents === "function") loadStudents();
    } catch (e) { toast("网络错误", "error"); }
}
if (qs("scoreTable")) loadScores();

// ===== 学科权重 =====
async function loadWeights() {
    try {
        var r = await fetch("/api/weights");
        var d = await r.json();
        var tb = qs("weightBody");
        if (d.length === 0) {
            tb.innerHTML = '<tr><td colspan="6"><div class="empty-state"><i class="bi bi-sliders"></i>暂无权重配置</div></td></tr>';
            return;
        }
        var h = "";
        for (var i = 0; i < d.length; i++) {
            var w = d[i];
            var b = (1 - w.weight).toFixed(2);
            h += '<tr><td><strong>' + esc(w.subject) + '</strong></td><td>' + (w.weight * 100).toFixed(0) + '%</td><td>' + (b * 100).toFixed(0) + '%</td><td>' + w.student_count + '</td>' +
                '<td><span class="formula-tag">总分 = ' + w.weight.toFixed(2) + 'x平时 + ' + b + 'x期末</span></td>' +
                '<td><button class="btn-danger-outline" data-action="deleteWeight" data-subject="' + esc(w.subject) + '"><i class="bi bi-trash"></i></button></td>' +
                "</tr>";
        }
        tb.innerHTML = h;
    } catch (e) { console.error(e); }
}

function showAddWeight() { qs("addWeightModal").classList.add("show"); }
function doAddWeight() {
    if (_submitting) return;
    _submitting = true;
    var f = qs("addWeightForm");
    var subject = f.querySelector("[name=subject]").value.trim();
    var weight = f.querySelector("[name=weight]").value;
    if (!subject) { _submitting = false; return; }
    fetch("/api/weights", { method: "POST", headers: {"Content-Type": "application/json"}, body: JSON.stringify({ subject: subject, weight: weight }) })
    .then(function(r) { return r.json(); })
    .then(function(j) {
        _submitting = false;
        if (j.error) { toast(j.error, "error"); return; }
        toast(subject + " 权重=" + weight);
        closeAddModal(); f.querySelector("[name=subject]").value = ""; f.querySelector("[name=weight]").value = "0.50";
        loadWeights(); loadStats();
    })
    .catch(function() { _submitting = false; toast("网络错误", "error"); });
}


async function deleteWeight(subject) {
    if (!confirm("确定删除 " + subject + " 的权重配置吗？")) return;
    try {
        var r = await fetch("/api/weights/" + encodeURIComponent(subject), { method: "DELETE" });
        var j = await r.json();
        if (!r.ok) { toast(j.error || "删除失败", "error"); return; }
        toast("已删除权重"); loadWeights(); loadStats();
    } catch (e) { toast("网络错误", "error"); }
}
if (qs("weightTable")) loadWeights();