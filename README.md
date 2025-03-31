BÁO CÁO BÀI TẬP LỚP MÔN LẬP TRÌNH NÂNG CAO
Mã số Sinh viên:  24020003                     
Họ và Tên: Mai Quốc An
Lớp Môn học: INT 2215 - 3
1.	Thông tin về game: link (github, google doc, youtube), game tham khảo (link, miêu tả) 
•	Tên game: Fishing Adventure
•	Nội dung game: người chơi điều khiển một chiếc tàu ngầm và cố gắng bắt càng nhiều cá càng tốt trong thời gian giới hạn.
•	Link Github: https://github.com/ancoto1234/gamecauca
2.	Các tính năng cải tiến trong game
•	Menu chính:
  o	Nền game với hình ảnh "Fishing Adventure"
  o	Hai nút "Start" và "Exit" để bắt đầu hoặc thoát game.
•	Gameplay:
  o	Điều khiển tàu và cần câu.
  o	Cá di chuyển ngẫu nhiên.
  o	Khi móc câu chạm vào cá, cá sẽ đi lên cùng móc câu.
  o	Khi cá lên đến tàu, điểm số tăng.
•	Hệ thống tính điểm:
  o	Sử dụng hình ảnh để hiển thị điểm số.
  o	Mỗi con cá bắt được cộng điểm và thời gian.
•	Đồng hồ đếm ngược:
  o	Thời gian bắt đầu từ 30 giây.
  o	Khi câu được cá, thời gian tăng thêm 2 giây.
•	Màn hình Game Over:
  o	Hiển thị chữ "Game Over".
  o	Người chơi có thể chọn chơi lại hoặc thoát bằng phím.
3.	Cấu trúc của project game: Tổ chức lớp, Miêu tả luồng, …
•	Tổ chức lớp:
  o	Fish : Đại diện cho cá trong game.
  o	FishingRod : Xử lý cần câu và móc kéo.
  o	Fisherman : Điều khiển nhân vật và thuyền.
  o	Game : Quản lý vòng lặp chính của game.
•	Luồng chương trình:
1.	Khởi tạo SDL2, tải hình ảnh, thiết lập game.
2.	Vòng lặp game:
  	Kiểm tra đầu vào từ bàn phím (di chuyển thuyền, thả móc câu).
  	Cập nhật vị trí cá, móc câu và điểm số.
  	Vẽ các đối tượng lên màn hình.
3.	Khi hết thời gian, game kết thúc.

4.	Nguồn tham khảo và lấy dữ liệu: âm thanh, đồ họa, chatgpt, 
  •	Đồ họa: Tải từ nguồn miễn phí như Freepik, OpenGameArt.
  •	Code tham khảo: SDL2 Documentation, ChatGPT hỗ trợ tối ưu code.
