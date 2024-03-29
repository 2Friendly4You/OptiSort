function sweetAlert(message, icon, timer = 3000, position = 'top-end') {
    Swal.mixin({
        toast: true,
        position: position,
        showConfirmButton: false,
        timer: timer,
        timerProgressBar: true,
        didOpen: (toast) => {
            toast.addEventListener('mouseenter', Swal.stopTimer)
            toast.addEventListener('mouseleave', Swal.resumeTimer)
        },
        color: 'var(--bs-light)',
        background: 'var(--bs-dark)'
    }).fire({
        icon: icon,
        title: message,
    });
}